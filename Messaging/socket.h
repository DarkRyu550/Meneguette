#pragma once

#include <stdbool.h>
#include <unistd.h>

/**
 * Wrapper over OS sockets, providing better support for multi-threaded access.
 *
 * Instances of this struct are reference counted and automatically closed when the reference
 * count reaches 0.
 */
typedef struct socket_t_internal socket_t;

/**
 * Wraps an existing socket. The returned socket has a reference count of 1.
 *
 * @param fd File descriptor of the socket to wrap.
 *
 * @return A new socket instance.
 */
socket_t* socket_create(int fd);

/**
 * Increases the reference count of a socket.
 *
 * @param s Socket to retain.
 */
void socket_retain(socket_t* s);

/**
 * Decreases the reference count of a socket.
 *
 * @param s Socket to release.
 */
void socket_release(socket_t* s);

/**
 * Reads data from a socket. If another thread already closed it, -1 is returned
 * and errno is set to ECONNRESET, otherwise the semantics are equivalent to read(2)
 *
 * @param s     Socket to read from.
 * @param buf   Buffer to write data to.
 * @param count Size of the buffer.
 *
 * @return On success, the number of bytes read (>=0), on error -1.
 */
ssize_t socket_read(socket_t* s, void* buf, size_t count);

/**
 * Writes data to a socket. If another thread already closed it, -1 is returned
 * and errno is set to ECONNRESET, otherwise the semantics are equivalent to write(2)
 *
 * @param s     Socket to write to.
 * @param buf   Buffer to read data from.
 * @param count Size of the buffer.
 *
 * @return On success, the number of bytes written (>=0), on error -1.
 */
ssize_t socket_write(socket_t* s, void* buf, size_t count);

/**
 * Marks a socket as unneeded, but does not yet close it. This is to prevent the (rare, unlikely
 * but still possible) race condition between a socket being closed and it's file descriptor being
 * reused for a different socket, while another thread might still be about to issue a system call
 * on the (old) file descriptor:
 *
 * Thread A    |    Thread B          |    Thread C
 *             | if(!closed)          |
 *  closed=1   |                      |
 *  close(fd)  |                      |
 *             |                      |  accept() //returns old value of `fd` because it has already been closed
 *             |   read(fd,buf,size)  |
 *
 * Leading to thread B reading data from the wrong socket
 *
 * All further reads/writes to this socket will fail, but only when the reference count reaches 0 the
 * file descriptor will be closed.
 *
 * @param s Socket to mark as unneeded.
 */
void socket_mark_unneeded(socket_t* s);

/**
 * Returns whether or not the socket should be disposed of. This is similar to a close, in that no further
 * reads or writes will succeed, but the resources haven't been released yet.
 *
 * Unneeded sockets should be released so their resources can be returned to the OS.
 *
 * @param s Socket to test.
 *
 * @return Whether or not the socket is unneeded anymore.
 */
bool socket_is_unneeded(socket_t* s);
