#include "address.h"
#include "endian.h"
#include "panic.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

/* Create a new IPv4 socket address. */
socket_address_t socket_address_ipv4(
	uint8_t a,
	uint8_t b,
	uint8_t c,
	uint8_t d,
	uint16_t port)
{
	socket_address_t address;

	address.ipv4.nibbles[0] = a;
	address.ipv4.nibbles[1] = b;
	address.ipv4.nibbles[2] = c;
	address.ipv4.nibbles[3] = d;
	address.ipv4.port = port;

	address.type = ADDRESS_TYPE_IPV4;

	return address;
}


/* Tries to create a socket connected to the given address. */
int socket_connect_tcp(socket_address_t address)
{
	int domain;
	switch(address.type)
	{
	case ADDRESS_TYPE_IPV4:
		domain = AF_INET;
		break;
	default:
		domain = 0;
		panic("Invalid socket address type: %d", address.type);
	}

	int fd = socket(domain, SOCK_STREAM, IPPROTO_TCP);
	if(fd < 0)
		panic("Could not create socket: (%d) %s", errno, strerror(errno));

	
	int result;
	if(address.type == ADDRESS_TYPE_IPV4)
	{
		struct sockaddr_in in;
		in.sin_family = AF_INET;
		in.sin_zero[0] = 0;
		in.sin_zero[1] = 0;
		in.sin_zero[2] = 0;
		in.sin_zero[3] = 0;
		in.sin_zero[4] = 0;
		in.sin_zero[5] = 0;
		in.sin_zero[6] = 0;
		in.sin_zero[7] = 0;
		in.sin_port = address.ipv4.port;

		uint32_t addr = 0;
		addr  |= address.ipv4.nibbles[0]; 
		addr <<= 8;
		addr  |= address.ipv4.nibbles[1]; 
		addr <<= 8;
		addr  |= address.ipv4.nibbles[2]; 
		addr <<= 8;
		addr  |= address.ipv4.nibbles[3];
		SetEndiannessFromNative(&addr, sizeof(uint32_t), ENDIANNESS_BIG);

		in.sin_addr.s_addr = addr;

		result = connect(fd, (struct sockaddr *)&in, sizeof(struct sockaddr_in));
	}
	else 
	{
		panic("Invalid socket address type: %d", address.type);
		result = -1;
	}

	if(result != 0)
		panic("Could not connect to requested address: (%d) %s", errno, strerror(errno));
	return fd;
}

/* Tries to create a socket bound to the given address. */
int socket_bind_tcp(socket_address_t address)
{
int domain;
	switch(address.type)
	{
	case ADDRESS_TYPE_IPV4:
		domain = AF_INET;
		break;
	default:
		domain = 0;
		panic("Invalid socket address type: %d", address.type);
	}

	int fd = socket(domain, SOCK_STREAM, IPPROTO_TCP);
	if(fd < 0)
		panic("Could not create socket: (%d) %s", errno, strerror(errno));

	
	int result;
	if(address.type == ADDRESS_TYPE_IPV4)
	{
		struct sockaddr_in in;
		in.sin_family = AF_INET;
		in.sin_zero[0] = 0;
		in.sin_zero[1] = 0;
		in.sin_zero[2] = 0;
		in.sin_zero[3] = 0;
		in.sin_zero[4] = 0;
		in.sin_zero[5] = 0;
		in.sin_zero[6] = 0;
		in.sin_zero[7] = 0;
		in.sin_port = address.ipv4.port;

		uint32_t addr = 0;
		addr  |= address.ipv4.nibbles[0]; 
		addr <<= 8;
		addr  |= address.ipv4.nibbles[1]; 
		addr <<= 8;
		addr  |= address.ipv4.nibbles[2]; 
		addr <<= 8;
		addr  |= address.ipv4.nibbles[3];
		SetEndiannessFromNative(&addr, sizeof(uint32_t), ENDIANNESS_BIG);

		in.sin_addr.s_addr = addr;

		result = bind(fd, (struct sockaddr *) &in, sizeof(struct sockaddr_in));
	}
	else
	{
		result = -1;
		panic("Invalid socket address type: %d", address.type);
	} 
	
	if(result != 0)
		panic("Could not connect to requested address: (%d) %s", errno, strerror(errno));
	return fd;
}
