module;
#include <cstdint>
#include <cstddef>

export module Endian;
export namespace Endian
{
	enum class Endianness
	{
		Big,
		Little,
	};

	/** Probe for the endianness of the currently running system. */
	Endianness SystemEndianness()
	{
		static_assert(sizeof(uint32_t) / sizeof(uint8_t) > 1);
		static_assert(sizeof(uint8_t) == 1);

		union Probe
		{
			uint32_t integer;
			uint8_t  bytes[sizeof(uint32_t)];
		};

		Probe p;
		p.integer = 0;

		for(size_t i = 0; i < sizeof(uint32_t); ++i)
		{
			p.integer  <<= sizeof(uint8_t) * 8;
			p.bytes[0] = 0xff;
		}

		if(p.bytes[sizeof(uint32_t) - 1] == 0xff)
			return Endianness::Little;
		else
			return Endianness::Big;
	}

	/** Operates on the  */
	template<typename T>
	T MakeNetworkOrder(T native)
	{
		static_assert(sizeof(T) / sizeof(uint8_t) > 1);
		static_assert(sizeof(uint8_t) == 1);

		union Probe
		{
			T value;
			uint8_t bytes[sizeof(T)];
		};

		if(SystemEndianness() == Endianness::Big)
			/* Already in network order. */
			return native;

		Probe p;
		p.value = native;

		for(size_t i = 0; i < sizeof(T) / 2; ++i)
		{
			uint8_t tmp;
			tmp = p.bytes[i];
			
			p.bytes[i] = p.bytes[sizeof(T) - i - 1];
			p.bytes[sizeof(T) - i - 1] = tmp;
		}

		return p.value;
	}
}