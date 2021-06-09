#include "message.h"
#include <string.h>

/* Create a new message structure from the given parameters. */
message_t message_create(const char *sender, const char *message)
{
	message_t target;
    const size_t max_sender_size = sizeof(target.sender) - 1;
    const size_t max_message_size = sizeof(target.message) - 1;

    memset(&target, 0, sizeof(target));

	size_t len = strlen(sender);
	if(len > max_sender_size) len = max_sender_size;

	memcpy(&target.sender[0], sender, len);

	len = strlen(message);
	if(len > max_message_size) len = max_message_size;

	memcpy(&target.message[0], message, len);

	return target;
}

/* Serialize the data of a message into a byte buffer, returning how many 
 * bytes of valid data were written. */
void message_serialize(const message_t* message, char buffer[512])
{
    const size_t sender_start = 0;
    const size_t message_start = sizeof(message->sender);

    memset(buffer, 0, 512 /* can't use sizeof here because it returns sizeof(char*) */);

    memcpy(&buffer[sender_start], message->sender, strlen(message->sender));
    memcpy(&buffer[message_start], message->message, strlen(message->message));
}

/* Deserialize the data in the given buffer, returning how many messages were
 * successfully deserialized. */
size_t message_deserialize(
	const char* buffer, 
	size_t len,
	message_t *messages,
	size_t max_messages)
{
	size_t message_count = len / 512;
	size_t offset = 0;

	if(message_count > max_messages)
		message_count = max_messages;

	for(size_t c = 0; c < message_count; ++c)
	{
		for(size_t i = 0; i < 64; ++i)
			messages[c].sender[i] = buffer[offset++];
		for(size_t i = 0; i < 448; ++i)
			messages[c].message[i] = buffer[offset++];
	}

	return message_count;
}
