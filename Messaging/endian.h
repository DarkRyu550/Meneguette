#ifndef __ENDIAN_H__
#define __ENDIAN_H__
#include <stddef.h>

enum
{
	ENDIANNESS_BIG    = 0,
	ENDIANNESS_LITTLE = 1,
};

/** Get the current endianness of the system we're running in. */
int GetSystemEndianness();

/** Put the given buffer, in native byte ordering, into the given order. */
void SetEndiannessFromNative(void *buffer, size_t len, int endianness);

#endif
