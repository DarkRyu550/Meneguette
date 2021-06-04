#include <iostream>
#include <cstddef>

import Net;

int main(void)
{
	Net::IpAddrV4 address({127, 0, 0, 1}, 24069);
	Net::TcpStream stream(address);

	uint8_t *buffer = new uint8_t[512];

	size_t read;
	while((read = stream.read(buffer, 512)) != 0)
	{
		for(size_t i = 0; i < read; ++i)
			std::cout << buffer[i];
	}

	delete[] buffer;
	return 0;
}