#include "message.h"
#include <string.h>

/* Create a new message structure from the given parameters. */
message_t message_create(const char *sender, const char *message)
{
	message_t target;

	size_t len = strlen(sender);
	if(len > 63) len = 63;

	memcpy(&target.sender[0], sender, len);
	target.sender[len] = 0;

	len = strlen(message);
	if(len > 447) len = 447;

	memcpy(&target.message[0], message, len);
	target.message[len] = 0;

	return target;
}

/* Access the data in the sender field. */
const char* message_sender(const message_t *message)
{
	return &message->sender[0];
}

/* Acces the data in the payload field. */
const char* message_payload(const message_t *message)
{
	return &message->message[0];
}

/* Serialize the data of a message into a byte buffer, returning how many 
 * bytes of valid data were written. */
void message_serialize(const message_t* message, char buffer[512])
{
    size_t offset = 0;
    size_t str = strlen(&message->sender[0]);
    for(size_t i = 0; i < 64; ++i)
    {
        if(i < str)
            buffer[offset++] = message->sender[i];
        else
            buffer[offset++] = 0;
    }

    str = strlen(&message->message[0]);
    for(size_t i = 0; i < 448; ++i)
    {
        if(i < str)
            buffer[offset++] = message->message[i];
        else
            buffer[offset++] = 0;
    }
}

/* Deserialize the data in the given buffe, returning how many messages were
 * successfuly deserialized. */
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
