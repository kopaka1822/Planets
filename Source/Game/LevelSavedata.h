#pragma once
#include "MapLoader.h"
#include "../Utility/FileReader.h"
#include "../Utility/FileWriter.h"
#include <algorithm>


class LevelSavedata
{
	struct LevelHeader
	{
		char sig[3];
		int Version;
		int LevepackID; // cheat protection
	};
public:
	LevelSavedata(const std::string& filename, int requiredID, bool ignoreId = false)
		:
		fullpath(filename)
	{
		try
		{
			FileReader r(filename);
			if (!r.isOpen())
				throw std::out_of_range("file does not exist");

			r.readStruct(&head);

			if (head.sig[0] != 'S' ||
				head.sig[1] != 'A' ||
				head.sig[2] != 'V')
				throw std::out_of_range("invalid file");

			if (head.Version != 1)
				throw std::out_of_range("invalid version");

			if (head.LevepackID != requiredID && !ignoreId)
				throw std::out_of_range("wrong id");

			// data
			for (int i = 0; i < 12; i++)
			{
				byte sflag = r.readChar();
				float time = r.readFloat();

				starflags.push_back(sflag);
				times.push_back(time);
			}

			// check ending
			char c1 = r.readChar();
			char c2 = r.readChar();
			char c3 = r.readChar();

			if (c1 != 'E' || c2 != 'N' || c3 != 'D')
				throw std::out_of_range("invalid end");
		}
		catch (const std::out_of_range& e)
		{
			if (!ignoreId)
				CreateEmptyFile(requiredID);
			else
				throw e;
		}
	}
	std::vector< byte >& GetStarflags()
	{
		return starflags;
	}
	std::vector< float >& GetTimes()
	{
		return times;
	}
	const std::vector< byte >& GetStarflags() const
	{
		return starflags;
	}
	const std::vector< float >& GetTimes() const
	{
		return times;
	}
	void SetNewRecord(int lvl, int flag, float time)
	{
		int mixed = starflags[lvl] | flag;
		starflags[lvl] = mixed;

		if (times[lvl] < 0)
			times[lvl] = time;
		else
			times[lvl] = std::min(times[lvl], time);

		Save();
	}
	void Save()
	{
		// save in file
		FileWriter w(fullpath);
		assert(w.isOpen());
		if (w.isOpen())
		{
			w.write<LevelHeader>(head);

			// write data
			for (int i = 0; i < 12; i++)
			{
				w.writeChar(starflags[i]);
				w.writeFloat(times[i]);
			}

			// write ending
			w.writeChar('E');
			w.writeChar('N');
			w.writeChar('D');
		}
	}
	int CountStars()
	{
		int count = 0;
		for (const auto& n : starflags)
		{
			if (n & 1)
				count++;
			if (n & 2)
				count++;
			if (n & 4)
				count++;
		}

		return count;
	}
private:
	void CreateEmptyFile(int saveid)
	{
		// Set Header
		head.sig[0] = 'S';
		head.sig[1] = 'A';
		head.sig[2] = 'V';
		head.Version = 1;
		head.LevepackID = saveid;

		// standart values
		starflags.clear();
		times.clear();

		starflags.assign(12, 0);
		times.assign(12, -1.0f);

		Save();
	}

private:
	LevelHeader head;
	std::vector< byte > starflags;
	std::vector< float > times;

	std::string fullpath;
};