#include "endian.h"
#include "panic.h"
#include <stddef.h>
#include <stdint.h>

_Static_assert(sizeof(uint8_t) == 1, 
	"Your implementation of <stdint.h> is non-conforming.");
_Static_assert(sizeof(uint32_t) == 4, 
	"Your implementation of <stdint.h> is non-conforming.");

union probe
{
	uint32_t integer;
	uint8_t nibbles[4];
};

int GetSystemEndianness()
{
	union probe probe;
	probe.integer = 0x10203040;
	if(probe.nibbles[0] == 0x10)
		return ENDIANNESS_BIG;
	else if(probe.nibbles[0] == 0x40)
		return ENDIANNESS_LITTLE;
	else
		panic("The current system has an unknown endianness.");
	return -1;
}

void SetEndiannessFromNative(void *buffer, size_t len, int endianness)
{
	switch(endianness)
	{
	case ENDIANNESS_BIG:
	case ENDIANNESS_LITTLE:
		break;
	default:
		panic("Expected either ENDIANNESS_BIG OR ENDIANNESS_LITTLE, got: %d", 
			endianness);
	}

	int native = GetSystemEndianness();
	if(native == endianness)
		/* Nothing to do. */
		return;


	size_t alignment = _Alignof(uint8_t);
	size_t buf_addrs = (size_t) buffer;
	if(buf_addrs % alignment != 0)
		panic("Buffer 0x%x is not correctly aligned to byte alignment %zu",
			buf_addrs,
			alignment);

	uint8_t *bytes = (uint8_t*) buffer;
	for(size_t i = 0; i < len / 2; ++i)
	{
		uint8_t tmp = bytes[i];
		bytes[i] = bytes[len - i - 1];
		bytes[len - i - 1] = tmp;
	}
}
