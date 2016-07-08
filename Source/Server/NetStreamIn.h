#pragma once
#include "../Utility/DataContainer.h"
#include <vector>

class NetStreamIn
{
public:
	NetStreamIn()
	{}
	// stream without head required!!
	// front ID will be the first 4 bytes if frontID >= 0
	// this could throw an std::logic_error if the stream is damaged
	std::vector<DataContainer> ProcessStream(DataContainer&& str, int frontID)
	{
		// extract stream
		std::vector<DataContainer> data;

		ContainerReader r(str);
		assert(r.remaining() > signed(sizeof(int)));

		size_t len = r.readInt();
		assert(len != 0);
		assert(len != 0xFFFFFFFF);

		if (last.length())
		{
			// something is in last, extract this first

			// append len bytes
			assert(r.remaining() >= len);
			if (r.remaining() < len)
				return data;

			last.append(r.readArray(len), len);

			assert(r.remaining() >= sizeof(int));
			if (r.remaining() < sizeof(int))
				return data;

			len = r.readInt();
		}

		while (r.remaining() >= 0)
		{
			if (len == 0)
			{
				// that was it
				data.push_back(std::move(last));
				return data;
			}
			if (len == 0xFFFFFFFF)
			{
				// data will be continued
				return data;
			}
			if (last.length())
			{
				data.push_back(std::move(last));
			}

			// put data in last
			last.clear();

			assert(r.remaining() >= len);
			if (r.remaining() < len)
				return data;

			if (frontID >= 0)
			{
				last += frontID;
			}
			last.append(r.readArray(len), len);

			assert(r.remaining() >= sizeof(int));
			if (r.remaining() < sizeof(int))
				return data;

			len = r.readInt();
		}
		return data;
	}
private:
	DataContainer last;
};
