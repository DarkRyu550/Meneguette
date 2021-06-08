#include "message_board.h"
#include "panic.h"
#include "address.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <threads.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

struct handler_bundle
{
	int socket;
	size_t connection_id;
	message_board_key* board_key;
};

static int receive_from_client(void *_bundle)
{
	int socket = ((struct handler_bundle*)_bundle)->socket;
	size_t connection_id = ((struct handler_bundle*)_bundle)->connection_id;
	message_board_key* board_key = ((struct handler_bundle*)_bundle)->board_key;
	free(_bundle);

	/* Current message buffer. */
	char buffer[512];
	/* Current received byte counter. */
	size_t bytes = 0;

	while(1)
	{
		ssize_t count_read = read(socket, &buffer[bytes], 512 - bytes);
		if(count_read < 0) 
		{
			fprintf(stderr, "Dropped connection %zu: (%d) %s\n",
				connection_id,
				errno,
				strerror(errno));
			break;
		}
		if(count_read == 0)
		{
			fprintf(stderr, "Dropped connection %zu: End of file (%zu in "
				"buffer).\n",
				connection_id, bytes);
			break;
		}
		bytes += count_read;

		if(bytes == 512)
		{
		    message_t msg;
			size_t deserialized = message_deserialize(buffer, bytes, &msg, 1);
			if(deserialized != 1)
				panic("This deserialization must have succeeded");

            message_board* board = message_board_acquire(board_key);
            message_board_add(board, &msg);
            message_board_release(board_key);

			fprintf(stderr, "Registered message from connection %zu\n", 
				connection_id);
			bytes = 0;
		}
	}
	close(socket);
	return 0;
}

static int send_to_client(void* _bundle) {
    int socket = ((struct handler_bundle*)_bundle)->socket;
    size_t connection_id = ((struct handler_bundle*)_bundle)->connection_id;
    message_board_key* board_key = ((struct handler_bundle*)_bundle)->board_key;
    free(_bundle);

    size_t message_cursor = 0;

    message_board* board = message_board_acquire(board_key);
    while(1) {
        message_t msg;
        while(message_board_poll(board, &message_cursor, &msg)) {
            char write_buffer[512];

            message_serialize(&msg, write_buffer);


            ssize_t written;
            if((written = write(socket, write_buffer, 512)) != 512)
            {
                fprintf(stderr, "Invalid write length of %zd bytes will"
                                "misalign communications. Terminating connection %zu.\n",
                        written, connection_id);
                message_board_release(board_key);
                goto end;
            }
        }
        message_board_wait(board);
    }
end:
    close(socket);
    return 0;
}

static void spawn_thread(int socket, size_t connection_id, message_board_key* board_key, thrd_start_t handler) {
    struct handler_bundle *bundle = malloc(sizeof(*bundle));
    if(bundle == NULL)
        panic("Could not allocate memory for thread initialization bundle");

    bundle->socket = socket;
    bundle->connection_id = connection_id;
    bundle->board_key = board_key;

    thrd_t thread;
    thrd_create(&thread, handler, bundle);
    thrd_detach(thread);
}

int main(void)
{
	socket_address_t address = socket_address_ipv4(127, 0, 0, 1, 24069);
	int socket = socket_bind_tcp(address);

	if(listen(socket, 1024) != 0)
		panic("Could not listen on bound socket: (%d) %s", errno, strerror(errno));

	int accepted = 0;
	int faults = 0;
	size_t connection = 0;

	//this is never freed, but the memory cannot be released anyway (new connections might need it),
	//so just let it get cleaned when the process dies (main thread stops accepting connections + client handling
	//threads exit)
	message_board_key* board_key = message_board_create();

	while(faults < 4)
	{
		accepted = accept(socket, NULL, NULL);
		if(accepted < 0)
		{
			fprintf(stderr, "Dropped a connection from accept(): (%d) %s\n",
				errno, strerror(errno));

			faults++;
			continue;
		}

        spawn_thread(accepted, connection, board_key, receive_from_client);
        spawn_thread(accepted, connection, board_key, send_to_client);
        connection++;
	}

	close(socket);
	return 0;
}
