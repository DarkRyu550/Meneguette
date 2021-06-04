#include <iostream>
#include <cstddef>
#include <memory>

import Net;

int main(void)
{
	auto address = std::make_shared<Net::IpAddrV4>(Net::IpAddrV4({127, 0, 0, 1}, 24069));
	Net::TcpListener listener(address);

	while(true)
	{
		Net::TcpStream stream = listener.accept();
		stream.write((uint8_t*) "Never Gonna Give You Up", 24);
	}
	return 0;
}