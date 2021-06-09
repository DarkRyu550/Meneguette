#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include "message_board.h"
#include "panic.h"

struct _message_board {
    message_t *messages;
    size_t message_count;
    size_t message_capacity;
    message_board_key* key; /* should not be freed, this is just an up-reference to the owner struct */
};

struct _message_board_key {
    mtx_t lock;
    cnd_t new_messages;
    message_board board;
};

message_board_key* message_board_create() {
    message_board_key* k = malloc(sizeof(*k));
    if(!k) {
        panic("Message board allocation failed");
    }
    if(mtx_init(&k->lock, mtx_plain | mtx_recursive) != thrd_success) {
        panic("Message board lock initialization failed");
    }
    if(cnd_init(&k->new_messages) != thrd_success) {
        panic("Message board condition variable initialization failed");
    }
    k->board.messages = NULL;
    k->board.message_count = 0;
    k->board.message_capacity = 0;
    k->board.key = k;
    return k;
}

void message_board_free(message_board_key* key) {
    int res;
    //5 tries to avoid spurious failures
    for(int i = 0; i < 5; i++) {
        if((res = mtx_trylock(&key->lock)) == thrd_busy) continue;
        break;
    }
    if(res != thrd_success) {
        panic("Failed to acquire mutex for freeing message board (free-while-used?), result=%s", res == thrd_busy ? "busy" : "error");
    }
    mtx_destroy(&key->lock);
    cnd_destroy(&key->new_messages);
    free(key->board.messages);
    free(key);
}

message_board* message_board_acquire(message_board_key* key) {
    if(mtx_lock(&key->lock) != thrd_success) {
        panic("Failed to acquire mutex");
    }
    return &key->board;
}

void message_board_release(message_board_key* key) {
    if(mtx_unlock(&key->lock) != thrd_success) {
        panic("Failed to release mutex");
    }
}

void message_board_add(message_board* board, const message_t* message) {
    if(board->message_count == board->message_capacity) {
        size_t new_size = board->message_capacity * 2;
        if(new_size == 0) new_size = 64;
        board->messages = realloc(board->messages, sizeof(message_t) * new_size);
        board->message_capacity = new_size;
        if(!board->messages) {
            panic("Failed to resize messages array");
        }
    }
    //write data to buffer
    memcpy(&board->messages[board->message_count], message, sizeof(message_t));
    board->message_count++;

    //notify any waiting listeners
    cnd_broadcast(&board->key->new_messages);
}

bool message_board_poll(message_board* board, message_board_cursor* cursor, message_t* message) {
    size_t idx = cursor->index;
    if(idx > board->message_count) {
        panic("Cursor ahead of message count (%zu / %zu)", idx, board->message_count);
    }
    //already up to date
    if(idx == board->message_count) {
        return false;
    }

    memcpy(message, &board->messages[idx], sizeof(message_t));

    //advance cursor
    cursor->index = idx + 1;
    return true;
}

void message_board_wait(message_board* board) {
    while(cnd_wait(&board->key->new_messages, &board->key->lock) != thrd_success) {
        ; //do nothing
    }
}
