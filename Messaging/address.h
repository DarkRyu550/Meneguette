#ifndef __ADDRESS_H__
#define __ADDRESS_H__
#include <stdint.h>

enum
{
	ADDRESS_TYPE_IPV4 = 1,
};

typedef struct
{
	int type;
	union
	{
		struct
		{
			uint8_t nibbles[4];
			uint16_t port;
		} ipv4;
	};
} socket_address_t;

/* Create a new IPv4 socket address. */
socket_address_t socket_address_ipv4(
	uint8_t a,
	uint8_t b,
	uint8_t c,
	uint8_t d,
	uint16_t port);

/* Tries to create a socket connected via TCP to the given address. */
int socket_connect_tcp(socket_address_t address);

/* Tries to create a socket bound via TCP to the given address. */
int socket_bind_tcp(socket_address_t address);

#endif
