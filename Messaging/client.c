#include "address.h"
#include "message.h"
#include "panic.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	socket_address_t address = socket_address_ipv4(127, 0, 0, 1, 24069);
	int socket = socket_connect_tcp(address);

	/* We'll need the standard input and the server socket in nonblocking mode 
	 * for later. */
	int stdin_fl = fcntl(STDIN_FILENO, F_GETFL);
	if(stdin_fl < 0)
		panic("Could not query FL of standard input: (%d) %s",
			errno, strerror(errno));
	if(fcntl(STDIN_FILENO, F_SETFL, stdin_fl | O_NONBLOCK) < 0)
		panic("Could not set the FL of standard input: (%d) %s",
			errno, strerror(errno));

	stdin_fl = fcntl(socket, F_GETFL);
	if(stdin_fl < 0)
		panic("Could not query FL of the socket: (%d) %s",
			errno, strerror(errno));
	if(fcntl(socket, F_SETFL, stdin_fl | O_NONBLOCK) < 0)
		panic("Could not set the FL of the socket: (%d) %s",
			errno, strerror(errno));
		
	
	/* Get us a sender name. */
	if(argc < 2)
		panic("Need at least one program argument giving the sender's name.");
	const char *sender = argv[1];

	char recv_buffer[512];
	char msg_buffer[448];
	char srl_buffer[512];

	size_t msg_bytes = 0;
	size_t recv_bytes = 0;

	ssize_t r;
	while(1)
	{
		r = read(socket, &recv_buffer[recv_bytes], 512 - recv_bytes);
		if(r <= 0)
		{
			if(errno != EAGAIN)
				/* Die on error. */
				break;
		}
		else recv_bytes += r;
		
		if(recv_bytes >= 512)
		{
			message_t msg;
			assert(message_deserialize(recv_buffer, 512, &msg, 1) == 1);
			recv_bytes = 0;

			fprintf(stdout, "%s> %s\n", msg.sender, msg.message);
		}

		while(msg_bytes < 448)
		{
			r = read(STDIN_FILENO, &msg_buffer[msg_bytes], 1);
			if(r <= 0)
			{
				if(errno == EAGAIN)
					/* We've got nothing to do. */
					break;
				else
					/* Die on error. */
					goto end;
			}
			msg_bytes += r;
		}

		if(msg_bytes >= 448 || (msg_bytes > 0 && msg_buffer[msg_bytes - 1] == '\n'))
		{
			msg_buffer[msg_bytes - 1] = 0;
			message_t msg = message_create(sender, msg_buffer);
			fprintf(stderr, "!! Sending message: %s\n", msg_buffer);

			assert(message_serialize(msg, srl_buffer, 512) == 1);
			if(write(socket, srl_buffer, 512) != 512)
			{
				fprintf(stderr, "Wrong write size would cause misaligned \
					communications. Dropping connection.\n");
				close(socket);
				return 1;
			}
		} 
	}
end:
	fprintf(stderr, "Main loop ended with errno=%d strerror=%s\n",
		errno, strerror(errno));

	close(socket);
	return 0;
}
