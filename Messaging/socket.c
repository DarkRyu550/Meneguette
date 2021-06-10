#include <errno.h>
#include <stdatomic.h>
#include <stdlib.h>
#include "panic.h"
#include "socket.h"

struct socket_t_internal {
    int fd;
    volatile atomic_bool unneeded;
    volatile atomic_size_t reference_count;
};

socket_t* socket_create(int fd) {
    socket_t* s = malloc(sizeof(*s));
    if(!s) {
        panic("Socket allocation failed");
    }
    s->fd = fd;
    s->unneeded = false;
    s->reference_count = 1;
    return s;
}

void socket_retain(socket_t* s) {
    atomic_fetch_add(&s->reference_count, 1);
}

void socket_release(socket_t* s) {
    if(atomic_fetch_sub(&s->reference_count, 1) == 1) {
        close(s->fd);
        free(s);
    }
}

ssize_t socket_read(socket_t* s, void* buf, size_t count) {
    if(atomic_load(&s->unneeded)) {
        errno = ECONNRESET;
        return -1;
    }
    return read(s->fd, buf, count);
}

ssize_t socket_write(socket_t* s, void* buf, size_t count) {
    if(atomic_load(&s->unneeded)) {
        errno = ECONNRESET;
        return -1;
    }
    return write(s->fd, buf, count);
}

void socket_mark_unneeded(socket_t* s) {
    atomic_store(&s->unneeded, true);
}

bool socket_is_unneeded(socket_t* s) {
    return atomic_load(&s->unneeded);
}
