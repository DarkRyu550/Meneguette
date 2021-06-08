module;
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <cstdint>
#include <string>
#include <sstream>
#include <array>
#include <memory>

import UnixExceptions;
import Endian;

export module Net;
export namespace Net
{
	class TcpStream;
	class TcpListener;

	class SocketAddr
	{
	protected:
		friend TcpStream;
		friend TcpListener;

		/** Get the socket address to connect to. */
		virtual const sockaddr * get_socket() const = 0;
		/** Get the length, in bytes, of the socket address. */
		virtual socklen_t get_socket_length() const = 0;
		/** Get the family to which this address belongs. */
		virtual int get_family() const = 0;
		/** Descirbe this address in human terms. */
		virtual std::string display() const = 0;
	};

	class IpAddrV4 : public SocketAddr
	{
	protected:
		/** Byte nibbles for the IPv4 address, in order. */
		uint_fast8_t nibbles[4];
		/** Target port number for TCP or UDP connections. */
		uint_fast16_t port;

		/** Actual data backing the Inet4 socket address. */
		sockaddr_in inet_socket;
	protected:
		virtual sockaddr const* get_socket() const
		{
			return reinterpret_cast<const sockaddr*>(&this->inet_socket);
		}

		virtual socklen_t get_socket_length() const
		{
			return sizeof(sockaddr_in);
		}

		virtual int get_family() const
		{
			return AF_INET;
		}

		virtual std::string display() const
		{
			std::stringstream target;
			target << (int) nibbles[0] << "." << (int) nibbles[1] << ".";
			target << (int) nibbles[2] << "." << (int) nibbles[3] << ":";
			target << (int) port;

			return target.str();
		}
	public:
		IpAddrV4(std::array<uint8_t, 4> nibbles, uint16_t port)
		{
			uint32_t address = 0;
			address  |= nibbles[0];
			address <<= 8;
			address  |= nibbles[1];
			address <<= 8;
			address  |= nibbles[2];
			address <<= 8;
			address  |= nibbles[3];

			this->nibbles[0] = nibbles[0];
			this->nibbles[1] = nibbles[1];
			this->nibbles[2] = nibbles[2];
			this->nibbles[3] = nibbles[3];
			this->port       = port;

			this->inet_socket.sin_family = AF_INET;
			this->inet_socket.sin_zero[0] = 0;
			this->inet_socket.sin_zero[1] = 0;
			this->inet_socket.sin_zero[2] = 0;
			this->inet_socket.sin_zero[3] = 0;
			this->inet_socket.sin_zero[4] = 0;
			this->inet_socket.sin_zero[5] = 0;
			this->inet_socket.sin_zero[6] = 0;
			this->inet_socket.sin_zero[7] = 0;
			this->inet_socket.sin_port = Endian::MakeNetworkOrder<in_port_t>(port);
			this->inet_socket.sin_addr.s_addr = Endian::MakeNetworkOrder<in_addr_t>(address);
		}
	};

	class TcpStream
	{
	protected:
		int fd;

	protected:
		friend TcpListener;

		/** Creates a new TCP stream from a file descriptor. */
		TcpStream(int fd)
		{
			this->fd = fd;
		}
	public:
		/** Tries to establish a TCP connection to the host at the given address. */
		TcpStream(const SocketAddr& address)
		{
			int family = address.get_family();
			fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
			if(fd < 0)
				UnixExceptions::RaiseFromErrno("while trying to create TCP socket");

			int error;
			error = connect(
				fd, 
				address.get_socket(), 
				address.get_socket_length());
			if(error)
			{
				std::stringstream what;
				what << "while trying to connect to address ";
				what << address.display();
				what << " via TCP";

				UnixExceptions::RaiseFromErrno(what.str());
			}
		}
		~TcpStream() 
		{
			if(fd >= 0)
				close(fd);
		}

		/** Write to this TCP socket. */
		size_t write(const uint8_t *data, size_t length)
		{
			return send(fd, data, length, 0);
		}

		/** Read from this TCP socket. */
		size_t read(uint8_t *data, size_t length)
		{
			return recv(fd, data, length, 0);
		}

		/* C++ Sucks. */
		TcpStream move()
		{
			int transfer_fd = this->fd;
			this->fd = -1;

			return TcpStream(transfer_fd);
		}
	};

	class TcpListener
	{
	protected:
		int fd;
		std::shared_ptr<SocketAddr> address;
	public:
		TcpListener(std::shared_ptr<SocketAddr> address)
		{
			this->address = address;

			int family = address->get_family();
			fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
			if(fd < 0)
				UnixExceptions::RaiseFromErrno("while trying to create TCP socket");

			int error;
			error = bind(
				fd, 
				address->get_socket(), 
				address->get_socket_length());
			if(error)
			{
				std::stringstream what;
				what << "while trying to bind to address ";
				what << address->display();
				what << " as a TCP listener";

				UnixExceptions::RaiseFromErrno(what.str());
			}
			listen(fd, 1024);
		}
		~TcpListener()
		{
			close(fd);
		}

		/** Accept the next client. */
		TcpStream accept()
		{
			int new_fd = accept4(
				fd,
				NULL,
				NULL,
				0);
			if(new_fd < 0)
			{
				std::stringstream what;
				what << "while trying to accept from address ";
				what << address->display();
				what << " as a TCP listener";

				UnixExceptions::RaiseFromErrno(what.str());
			}

			return TcpStream(new_fd);
		}
	};
}