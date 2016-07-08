#pragma once
#include "MapLoader.h"
#include <memory>
#include "../Utility/GameError.h"
#include "LevelSavedata.h"

class Levelpack
{
	struct PackHeader
	{
		char sig[3];
		int Version;
		int ID;
	};

	static const int curVersion = 1;
public:
	Levelpack(Levelpack&& m)
	{
		Swap(m);
	}
	Levelpack& operator=(Levelpack&& m)
	{
		Swap(m);
	}
	Levelpack(const std::string& filename)
		:
		packname(filename)
	{
		// loading
		FILE* pFile = fopen(filename.c_str(), "rb");
		if (!pFile)
		{
			MakeInvalid();
			return;
		}

		fread(&header, sizeof(PackHeader), 1, pFile);
		// check header etc
		if (!isValid())
		{
			fclose(pFile);
			return;
		}
		if (header.Version != curVersion)
		{
			MakeInvalid();
			fclose(pFile);
			return;
		}
		numLvls = 12; // in cur version

		// load times
		times.assign(12, 0.0f);
		fread(&times[0], sizeof(float) * 12, 1, pFile);

		// load maps
		for (int i = 0; i < 12; i++)
		{
			maps.push_back(MapLoader(pFile));
			if (!maps[i].isValid())
			{
				MakeInvalid();
				fclose(pFile);
				return;
			}
		}

		// loaded succesfully
		fclose(pFile);

		// Load or create savedata
		// remove extension
		std::string savepath = tool::cutStringRight(filename, '.');
		savepath = tool::fileAddExtension(savepath, "sav");

		pSavedata = std::unique_ptr< LevelSavedata >(new LevelSavedata(savepath, header.ID));
	}
	bool isValid()
	{
		if (header.sig[0] == 'P' && header.sig[1] == 'C' && header.sig[2] == 'K')
			return true;
		return false;
	}
	// lvl 0 - 11
	const MapLoader& GetLevel(int lvl) const
	{
		return maps[lvl];
	}
	const size_t GetNumLevels() const
	{
		return numLvls;
	}
	float GetLevelTime(int lvl) const
	{
		return times[lvl];
	}
	std::string GetName() const
	{
		return packname;
	}
	LevelSavedata& GetSavedata()
	{
		return *pSavedata;
	}
	// Saving
	/*
		filename: levelpack1.pack
		mapfiles: data/maps/leve1.map ...
		times in secods: 120s ...

		throws GameError
	*/
	static void Save(const std::string& filename, const std::vector<std::string>& mapnames, const std::vector< float > maptimes)
	{
		assert(maptimes.size() == 12);
		assert(mapnames.size() == 12);

		PackHeader head;
		head.Version = curVersion;
		head.ID = rand();
		head.sig[0] = 'P';
		head.sig[1] = 'C';
		head.sig[2] = 'K';

		FILE* pFile = fopen(filename.c_str(), "wb");
		if (!pFile)
			throw GameError("could not save MapPack: fopen failed");

		// write header
		fwrite(&head, sizeof(PackHeader), 1, pFile);

		// write times
		fwrite(&maptimes[0], sizeof(maptimes[0]) * maptimes.size(), 1, pFile);

		// write maps
		for (int i = 0; i < 12; i++)
		{
			MapLoader l(mapnames[i]);
			if (!l.isValid())
			{
				fclose(pFile);
				throw GameError("MapPack Mapname invalid: " + mapnames[i]);
			}

			if (!MapLoader::SaveMap(pFile, l.GetPlanets(), l.GetSpawns(), l.GetPlayers(), l.GetWidth(), l.GetHeight(), l.GetTeams(), l.GetPassword()))
			{
				fclose(pFile);
				throw GameError("MapPack Map could not be written: " + mapnames[i]);
			}
		}

		fclose(pFile);
		// saved succesfully
	}
private:
	void MakeInvalid()
	{
		header.sig[0] = 0;
	}
	void Swap(Levelpack& o)
	{
		std::swap(maps, o.maps);
		std::swap(times, o.times);
		std::swap(numLvls, o.numLvls);
		std::swap(packname, o.packname);
		std::swap(header, o.header);
		std::swap(pSavedata, o.pSavedata);
	}
private:
	std::vector< MapLoader > maps;
	std::vector< float > times;
	size_t numLvls = -1;
	std::string packname;

	PackHeader header;

	std::unique_ptr< LevelSavedata > pSavedata;
};