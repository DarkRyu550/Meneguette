#include "message.h"
#include "panic.h"
#include "address.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <threads.h>
#include <stdatomic.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/* The message board structure. */
static struct
{
	mtx_t board_lock;

	message_t *messages;
	size_t message_count;
	size_t message_capacity;
} MessageBoard;
static _Atomic size_t MessageBoard_message_count;

struct handler_bundle
{
	int socket;
	size_t connection_id;
};

int handle(void *_bundle)
{
	int socket = ((struct handler_bundle*)_bundle)->socket;
	size_t connection_id = ((struct handler_bundle*)_bundle)->connection_id;
	free(_bundle);

	/* Current position of the message cursor. */
	static _Thread_local size_t message_cursor;
	/* Current message buffer. */
	static _Thread_local char buffer[512];
	/* Current received byte counter. */
	static _Thread_local size_t bytes;

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
			/* Register the next message. */
			if(mtx_lock(&MessageBoard.board_lock) != thrd_success)
				panic("Could not lock message board!");
			
			message_t msg;
			size_t deserialized = message_deserialize(buffer, bytes, &msg, 1);
			if(deserialized != 1)
				panic("This deserialization must have succeeded");

			
			size_t message_count = atomic_fetch_add(&MessageBoard_message_count, 1);
			while(message_count >= MessageBoard.message_capacity)
			{
				MessageBoard.message_capacity *= 2;
				if(MessageBoard.message_capacity == 0)
					MessageBoard.message_capacity = 1;

				MessageBoard.messages = realloc(
					MessageBoard.messages,
					MessageBoard.message_capacity);
				if(MessageBoard.messages == NULL)
					panic("Could not reallocate memory for %zu messages", 
						MessageBoard.message_capacity);
			}
			printf("%zu\n", message_count);
			MessageBoard.messages[message_count] = msg;

			if(mtx_unlock(&MessageBoard.board_lock) != thrd_success)
				panic("Could not unlock message board!");

			fprintf(stderr, "Registered message from connection %zu\n", 
				connection_id);
			bytes = 0;
		}

		/* Pull in any messages we don't know about. */
		size_t message_count = atomic_load(&MessageBoard_message_count);
		if(message_count != message_cursor)
		{
			if(mtx_lock(&MessageBoard.board_lock) != thrd_success)
				panic("Could not lock message board!");

			while(message_cursor < message_count)
			{
				char write_buffer[512];
				message_t tbw = MessageBoard.messages[message_cursor];

				if(message_serialize(tbw, write_buffer, 512) != 512)
					panic("This could not have failed");


				ssize_t written;
				if((written = write(socket, write_buffer, 512)) != 512)
				{
					fprintf(stderr, "Invalid write length of %zd bytes will"
						"misalign communications. Terminating connection %zu.\n",
						written, connection_id);
					goto end;
				}

				message_cursor++;
			}

			if(mtx_unlock(&MessageBoard.board_lock) != thrd_success)
				panic("Could not unlock message board!");
		}
	}

end:
	close(socket);
	return 0;
}

int main(void)
{
	atomic_init(&MessageBoard_message_count, 0);

	socket_address_t address = socket_address_ipv4(127, 0, 0, 1, 24069);
	int socket = socket_bind_tcp(address);

	if(listen(socket, 1024) != 0)
		panic("Could not listen on bound socket: (%d) %s", errno, strerror(errno));

	int accepted = 0;
	int faults = 0;
	size_t connection = 0;

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

		struct handler_bundle *bundle = malloc(sizeof(*bundle));
		if(bundle == NULL)
			panic("Could not allocate memory for thread initialization bundle");

		bundle->socket = accepted;
		bundle->connection_id = connection++;

		thrd_t thread;
		thrd_create(&thread, handle, bundle);
		thrd_detach(thread);
	}

	close(socket);
	return 0;
}
