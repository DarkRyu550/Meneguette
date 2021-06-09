#pragma once

#include <stdbool.h>
#include "message.h"

/* Identifies a message board, such that it can be shared between threads */
typedef struct _message_board_key message_board_key;

/* Represents exclusive access to a message board, can only be obtained after locking */
typedef struct _message_board message_board;

typedef struct { size_t index; } message_board_cursor;

/**
 * Allocates a new message board.
 *
 * @return A handle to the message board.
 *
 * @see message_board_acquire
 */
message_board_key* message_board_create();

/**
 * Deletes a message board, releasing all resources held by it.
 *
 * @param key Handle to the message board that should be deleted.
 */
void message_board_free(message_board_key* key);

/**
 * Acquires exclusive access to a board. The returned pointer is guaranteed to not be used
 * by other threads until the matching `message_board_release` call.
 *
 * Using the pointer after the matching release call is *undefined behavior*.
 *
 * @param key board to acquire.
 *
 * @return Access to the board's data.
 */
message_board* message_board_acquire(message_board_key* key);

/**
 * Releases access to a given board.
 *
 * @param key board to release.
 */
void message_board_release(message_board_key* key);

/**
 * Adds a message to the board. The message is *copied*, so the given pointer is
 * safe to free after this function returns.
 */
void message_board_add(message_board* board, const message_t* message);

/**
 * Attempts to read a message from the board, given the last read position.
 *
 * `cursor` should be initialized to 0 before the first call, then reused for the same
 * client.
 *
 * @param board   board to read messages from.
 * @param cursor  current message cursor, updated on reads.
 * @param message where to store the read message, unmodified if up-to-date.
 *
 * @return whether or not a message was read.
 */
bool message_board_poll(message_board* board, message_board_cursor* cursor, message_t* message);

/**
 * Waits until new messages are available on the board. May return spuriously.
 *
 * Releases ownership of the board while waiting.
 *
 * @param board board to wait for.
 */
void message_board_wait(message_board* board);
