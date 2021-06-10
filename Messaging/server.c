#include "message_board.h"
#include "panic.h"
#include "address.h"
#include "socket.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <threads.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

struct handler_bundle
{
	socket_t* socket;
	size_t connection_id;
	message_board_key* board_key;
};

static int receive_from_client(void *_bundle)
{
    socket_t* socket = ((struct handler_bundle*)_bundle)->socket;
	size_t connection_id = ((struct handler_bundle*)_bundle)->connection_id;
	message_board_key* board_key = ((struct handler_bundle*)_bundle)->board_key;
	free(_bundle);

	/* Current message buffer. */
	char buffer[512];
	/* Current received byte counter. */
	size_t bytes = 0;

	while(1)
	{
		ssize_t count_read = socket_read(socket, &buffer[bytes], 512 - bytes);
		if(socket_is_unneeded(socket)) {
            fprintf(stderr, "Dropped connection %zu: Socket closed by writer thread (%zu in "
                            "buffer).\n",
                    connection_id, bytes);
            break;
		}
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

		//messages are exactly 512 bytes (padded to length as needed), so until an entire message has been read
		//there's nothing to do.
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
    message_board_ref_release(board_key);
    socket_mark_unneeded(socket);
    socket_release(socket);
	return 0;
}

static int send_to_client(void* _bundle) {
    socket_t* socket = ((struct handler_bundle*)_bundle)->socket;
    size_t connection_id = ((struct handler_bundle*)_bundle)->connection_id;
    message_board_key* board_key = ((struct handler_bundle*)_bundle)->board_key;
    free(_bundle);

    message_board_cursor message_cursor = {0};

    message_board* board = message_board_acquire(board_key);
    while(1) {
        //if socket marked unneeded by reader, bail out
        if(socket_is_unneeded(socket)) {
            break;
        }

        message_t msg;
        //while there are still messages not sent, send them to the client.
        //most of the time, only a single message will be read, after the initial
        //burst when a client connects (if there were messages sent before that).
        while(message_board_poll(board, &message_cursor, &msg)) {
            char write_buffer[512];

            message_serialize(&msg, write_buffer);


            ssize_t written;
            if((written = socket_write(socket, write_buffer, 512)) != 512)
            {
                //don't print anything if socket_is_unneeded is true, because it'll only return
                //that if the reader thread sets it, in which case it already printed the socket close.
                if(!socket_is_unneeded(socket)) {
                    fprintf(stderr, "Invalid write length of %zd bytes will "
                                    "misalign communications. Terminating connection %zu.\n",
                            written, connection_id);
                }
                goto end;
            }
        }
        //wait for new messages to be added
        message_board_wait(board);
    }
end:
    message_board_release(board_key);
    message_board_ref_release(board_key);

    socket_mark_unneeded(socket);
    socket_release(socket);
    return 0;
}

static void spawn_worker_thread(socket_t* socket, size_t connection_id, message_board_key* board_key, thrd_start_t handler) {
    //a new bundle is allocated for each thread, even though both have the same data,
    //because this greatly simplifies freeing them, as each thread can just free() as soon
    //as it's done copying the values, without needing to synchronize with other threads
    //for cleanup.
    struct handler_bundle *bundle = malloc(sizeof(*bundle));
    if(bundle == NULL)
        panic("Could not allocate memory for thread initialization bundle");

    //increase reference counts for the new thread
    socket_retain(socket);
    message_board_ref_retain(board_key);

    bundle->socket = socket;
    bundle->connection_id = connection_id;
    bundle->board_key = board_key;

    thrd_t thread;
    if(thrd_create(&thread, handler, bundle) != thrd_success) {
        panic("Failed to create thread");
    }
}

int main(void)
{
	socket_address_t address = socket_address_ipv4(0, 0, 0, 0, 3621);
	int socket = socket_bind_tcp(address);

	if(listen(socket, 1024) != 0)
		panic("Could not listen on bound socket: (%d) %s", errno, strerror(errno));

	int accepted;
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

		socket_t* s = socket_create(accepted);
        spawn_thread(s, connection, board_key, receive_from_client);
        spawn_thread(s, connection, board_key, send_to_client);
        socket_release(s);

        connection++;
	}

	close(socket);
    message_board_ref_release(board_key);
	return 0;
}
