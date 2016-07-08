#pragma once
#include "MusicFile.h"
#include <vector>
#include <memory>
#include "Sound.h"

class ThemeMixer
{
public:
	ThemeMixer(const std::vector< std::unique_ptr< MusicFile >>& titles)
		:
		files(titles),
		nFiles(titles.size())
	{
		for (auto& m : titles)
		{
			//m->SetMute(true);
			m->Play(Sound::GetNextFreeChannel(), true);
		}
	}
	// 0 = nothing
	void Play(unsigned int lvl)
	{
		mask = 0;
		for (unsigned int i = 0; i < lvl; ++i)
		{
			mask = mask << 1;
			++mask;
		}
		Play();
	}
	unsigned int MaxLvl() const
	{
		return nFiles;
	}
	void Stop()
	{
		Play(0);
	}
	unsigned int GetMask() const
	{
		return mask;
	}
	void SetMask(unsigned int m)
	{
		mask = m;
		Play();
	}
	void SetFilter(unsigned int f)
	{
		filter = f;
		Play();
	}
	unsigned int GetFilter() const
	{
		return filter;
	}
private:
	void Play()
	{
		unsigned int ma = mask & filter;

		for (auto& m : files)
		{
			if (ma & 1) //first bit is one
			{
				m->SetMute(false);
			}
			else
			{
				m->SetMute(true);
			}

			ma = ma >> 1;
		}
	}

private:
	const std::vector<std::unique_ptr<MusicFile>>& files;
	const unsigned int nFiles;

	unsigned int mask = 0;
	unsigned int filter = 0xFF;
};