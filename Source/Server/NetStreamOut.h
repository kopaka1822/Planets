#pragma once
#include "../Utility/DataContainer.h"
#include "../Utility/Timer.h"
class NetStreamOut
{
public:
	NetStreamOut(size_t header, size_t maxSize)
		:
		HEAD(header),
		MAX_SIZE(maxSize)
	{
		ResetCurStream();
	}

	// time in ms
	float GetTimeSinceFirst() const
	{
		if (curStream.length() == HEAD)
			return 0.0f;

		return addTime.GetTimeMilli();
	}
	// returns true if buffer is full and should be send
	bool AddData(DataContainer&& con)
	{
		assert(curStream.length() >= HEAD);
		if (curStream.length() == HEAD)
		{
			addTime.StartWatch();
		}
		if (con.length() == 0)
			return false;

		size_t elen = curStream.length() + sizeof(int) + con.length() + sizeof(int);
		if (elen >= MAX_SIZE)
		{
			// split!
			assert(MAX_SIZE - curStream.length() >= 3 * sizeof(int));

			// calculate appending size
			size_t remaining = MAX_SIZE - curStream.length();
			remaining = remaining - 2 * sizeof(int);

			curStream += (int)remaining;
			curStream.append(con.start(), remaining);
			curStream += (int)0xFFFFFFFF; // - 1 = stream continue in next

			notAppended.clear();
			assert(con.length() - remaining > 0);
			notAppended.append(con[remaining], con.length() - remaining);
			bEnd = true;
			return true;
		}
		else
		{
			// append normal
			curStream += (int)con.length();
			curStream.append(con.start(), con.length());

			if (MAX_SIZE - curStream.length() <= 3 * sizeof(int))
			{
				// finish stream!
				curStream += (int)0; // 0 = stream end
				notAppended.clear();
				bEnd = true;
				return true;
			}
			return false;
		}
	}
	bool GetCurStream(DataContainer* dst)
	{
		if (curStream.length() <= HEAD) // no data
			return false;

		if (!bEnd)
		{
			// write end bytes
			curStream += (int)0; // 0 = stream end
			notAppended.clear();
		}
		// move old
		*dst = std::move(curStream);
		ResetCurStream(); // new header etc.

		// old data to append?
		if (notAppended.length())
		{
			AddData(std::move(notAppended));
		}
		return true;
	}
	void ResetMax(size_t sz)
	{
		MAX_SIZE = sz;
	}
private:
	void ResetCurStream()
	{
		curStream.clear();
		curStream.reserve(MAX_SIZE + 1);
		curStream.skip(HEAD);
		bEnd = false;
	}
private:
	const size_t HEAD;
	size_t MAX_SIZE;
	DataContainer curStream; // cur Container data
	DataContainer notAppended; // data that was not appended in the last stream
	Timer addTime;
	bool bEnd = false;
};
