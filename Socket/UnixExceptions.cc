module;
#include <errno.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sstream>

export module UnixExceptions;
export namespace UnixExceptions
{
	/** Generic, root Unix exception. */
	class UnixException : public std::runtime_error
	{
	protected:
		int _errno;
	public:
		UnixException(int _errno)
			: std::runtime_error("No information given")
		{ 
			this->_errno = _errno; 
		}

		UnixException(int _errno, std::string what)
			: std::runtime_error(what)
		{
			this->_errno = _errno;
		}

		/** Return the inner error number backing this exception. */
		int get_errno() const { return _errno; }

		const char* what() const noexcept 
		{
			std::stringstream target;
			target << "(" << _errno << ") " << strerror(_errno) << ": ";
			target << std::runtime_error::what();

			/* Since what() doesn't let us return an owned string, leaking
			 * here is the best that we can do to guarantee the composed message
			 * remains valid for the rest of the life of the program. */
			char* message = new char[target.str().size()];
			memcpy(message, target.str().c_str(), target.str().size());

			return message;
		}
	};

	/** Error type for missing entries. Corresponds to ENOENT. */
	class NoSuchEntryException : public UnixException
	{
	public:
		NoSuchEntryException()
			: UnixException(ENOENT) { }
		NoSuchEntryException(std::string what)
			: UnixException(ENOENT, what) { }
	};

	/** Error type for refused connections. Corresponds to ECONNREFUSED. */
	class ConnectionRefused : public UnixException
	{
	public:
		ConnectionRefused()
			: UnixException(ECONNREFUSED) { }
		ConnectionRefused(std::string what)
			: UnixException(ECONNREFUSED, what) { }
	};

	/** Raises the current value of the `errno` as an exception. */
	void RaiseFromErrno(std::string what)
	{
		switch(errno)
		{
		case ENOENT:
			throw NoSuchEntryException(what); 
			break;
		case ECONNREFUSED:
			throw ConnectionRefused(what);
			break;
		default:
			throw UnixException(errno, what);
			break;
		}
	}
}