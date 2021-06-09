#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include <stddef.h>

typedef struct 
{
	char sender[64];
	char message[448];
} message_t;

/* Create a new message structure from the given parameters. */
message_t message_create(const char *sender, const char *message);

/* Serialize the data of a message into a byte buffer, returning how many 
 * bytes of valid data were written. */
void message_serialize(const message_t* message, char buffer[512]);

/* Deserialize the data in the given buffer, returning how many messages were
 * successfully deserialized. */
size_t message_deserialize(
	const char* buffer, 
	size_t len,
	message_t *messages,
	size_t max_messages);

#endif
