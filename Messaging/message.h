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

/* Access the data in the sender field. */
const char* message_sender(message_t *message);

/* Acces the data in the payload field. */
const char* message_payload(message_t *message);

/* Serialize the data of a message into a byte buffer, returning how many 
 * bytes of valid data were written. */
size_t message_serialize(message_t message, char *buffer, size_t len);

/* Deserialize the data in the given buffe, returning how many messages were
 * successfuly deserialized. */
size_t message_deserialize(
	const char* buffer, 
	size_t len,
	message_t *messages,
	size_t max_messages);

#endif
