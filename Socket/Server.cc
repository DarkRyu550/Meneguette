#include <iostream>
#include <cstddef>
#include <memory>
#include <thread>
#include <atomic>

import Net;

void HandleClient(Net::TcpStream stream)
{
	stream.write((uint8_t*) "Never Gonna Give You Up", 24);
}

int main(void)
{
	auto address = std::make_shared<Net::IpAddrV4>(Net::IpAddrV4({127, 0, 0, 1}, 24069));
	Net::TcpListener listener(address);

	while(true)
	{
		Net::TcpStream stream = listener.accept();
		std::thread thread([&stream]()
		{
			HandleClient(stream.move());
		});
		thread.detach();
	}

	return 0;
}
