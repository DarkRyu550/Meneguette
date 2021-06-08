module;
#include <cstddef>
#include <cstdio>

export module Message;
export namespace Message
{
	class Message
	{
	public:
		void serialize(W& target)
		{
		}

		static Message deserialize(R& source)
		{

		}
	};
}
