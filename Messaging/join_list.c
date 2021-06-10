#include <stdlib.h>
#include "join_list.h"
#include "panic.h"

struct join_list_internal {
    mtx_t lock;
    cnd_t not_empty;

    thrd_t* data;
    size_t size;
    size_t capacity;

    size_t total_threads;
    size_t processed_threads;

    bool terminating;
};

static void lock(join_list* list) {
    if(mtx_lock(&list->lock) != thrd_success) {
        panic("Join list lock failed");
    }
}
static void unlock(join_list* list) {
    if(mtx_unlock(&list->lock) != thrd_success) {
        panic("Join list unlock failed");
    }
}

join_list* join_list_new() {
    join_list* l = malloc(sizeof(*l));
    if(!l) {
        panic("Join list allocation failed");
    }
    if(mtx_init(&l->lock, mtx_plain) != thrd_success) {
        panic("Join list mutex init failed");
    }
    if(cnd_init(&l->not_empty) != thrd_success) {
        panic("Join list condition variable init failed");
    }

    l->data = NULL;
    l->size = 0;
    l->capacity = 0;

    l->total_threads = 0;
    l->processed_threads = 0;

    l->terminating = false;

    return l;
}

void join_list_free(join_list* list) {
    free(list->data);
    mtx_destroy(&list->lock);
    cnd_destroy(&list->not_empty);
    free(list);
}

bool join_list_next(join_list* list, thrd_t* thread) {
    lock(list);

    while(list->size == 0) {
        if(list->terminating && list->total_threads == list->processed_threads) {
            return false;
        }
        cnd_wait(&list->not_empty, &list->lock);
    }

    *thread = list->data[list->size - 1];
    list->size--;
    list->processed_threads++;

    unlock(list);

    return true;
}

void join_list_start_termination(join_list* list) {
    lock(list);
    list->terminating = true;
    unlock(list);
}

void join_list_notify_creation(join_list* list) {
    lock(list);
    if(list->terminating) {
        panic("Threads cannot be added once termination is started");
    }
    list->total_threads++;
    unlock(list);
}

void join_list_notify_exit(join_list* list, thrd_t thread) {
    lock(list);

    if(list->size == list->capacity) {
        size_t new_cap = list->capacity + 10;
        list->data = realloc(list->data, new_cap * sizeof(thrd_t));
        if(!list->data) {
            panic("Failed to resize list");
        }
        list->capacity = new_cap;
    }

    list->data[list->size] = thread;
    list->size++;
    cnd_signal(&list->not_empty);

    unlock(list);
}
