#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "GameTypes.h"
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#ifndef M_PI
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <memory>
#include <string.h>

typedef unsigned char byte;

class MapLoader
{
	// 2 = without entity types
	static const int curVersion = 4;
private:
	struct MapHeader
	{
		char sig[3];
		byte nTeams;
		byte nPlanets;
		byte nSpawns;
		byte Version;
		float width;
		float height;
		bool isProtected; //has password?
		char passwd[10];
	};
	struct MapEnd
	{
		char sig[3];
	};
public:
	enum EntityType
	{
		E_NORMAL,
		E_BOMBER,
		E_SPEEDER,
		E_SABOTEUR
	};
	struct MapPlanet
	{
		float x;
		float y;
		byte team;
		float radius;
		float sUnit;
		int HP;
	};
	struct MapSpawn
	{
		float x, y;
		byte team;
		int nUnits;
		byte type;
	};
	struct MapEntity
	{
		float x, y;
		byte team;
		byte type;
	};
public:
	MapLoader(const std::string& filename)
	{
		//fill structures
		FILE* pFile = nullptr;
		pFile = fopen(filename.c_str(), "rb");

		if (pFile == nullptr)
		{
			MakeInvalid();
			return;
		}

		init(pFile);

		fclose(pFile);
	}
	MapLoader(FILE* pFile)
	{
		init(pFile);
	}
	bool isValid() const
	{
		return (header.sig[0] == 'M' && header.sig[1] == 'A' && header.sig[2] == 'P');
	}
	bool isProteced() const
	{
		return header.isProtected;
	}
	bool TryPassword(const std::string& pass) const
	{
		return pwd == pass;
	}
	std::string GetPassword() const
	{
		return pwd;
	}
	byte GetPlayers() const
	{
		return header.nTeams;
	}
	const std::vector< MapPlanet >& GetPlanets() const
	{
		return planets;
	}
	const std::vector< MapSpawn >& GetSpawns() const
	{
		return spawns;
	}
	float GetWidth() const
	{
		return header.width;
	}
	float GetHeight() const
	{
		return header.height;
	}
	static bool SaveMap(const std::string& file, const std::vector< MapPlanet >& p,
		const std::vector< MapSpawn >& s, TeamID nTeams,
		float width, float height, const std::vector< TeamID >& teams = std::vector< TeamID >(), std::string pass = std::string())
	{
		FILE* pFile = nullptr;
		pFile = fopen(file.c_str(), "wb");

		if (pFile == nullptr)
			return false;

		SaveMap(pFile, p, s, nTeams, width, height, teams, pass);

		fclose(pFile);
		return true;
	}

	static bool SaveMap(FILE* pFile, const std::vector< MapPlanet >& p,
		const std::vector< MapSpawn >& s, TeamID nTeams,
		float width, float height, const std::vector< TeamID >& teams, std::string pass = std::string())
	{
		//Write header
		MapHeader head;
		head.nPlanets = static_cast<byte>(p.size());
		head.nSpawns = static_cast<byte>(s.size());
		head.nTeams = nTeams;
		head.sig[0] = 'M';
		head.sig[1] = 'A';
		head.sig[2] = 'P';
		head.Version = curVersion;
		head.width = width;
		head.height = height;
		head.isProtected = (pass.length() != 0);

		if (head.passwd)
		{
			//assert(pass.length() <= 9);
			auto len = pass.length();
			if (len > 9) len = 9;
			for (unsigned int i = 0; i < len; ++i)
			{
				head.passwd[i] = ~pass[i];
				head.passwd[i + 1] = '\0';
			}

		}

		fwrite(&head, sizeof(MapHeader), 1, pFile);

		//write n Planets
		for (const auto pl : p)
		{
			fwrite(&pl, sizeof(MapPlanet), 1, pFile);
		}
		//write n Ents
		for (const auto sp : s)
		{
			fwrite(&sp, sizeof(MapSpawn), 1, pFile);
		}

		// write teams
		if (teams.size() && TestTeams(teams,nTeams))
		{
			char t = 1;
			// teams allowed
			fwrite(&t, sizeof(bool), 1, pFile);
			if (teams.size() < nTeams)
				return false;

			for (size_t i = 0; i < nTeams; i++)
			{
				fwrite(&teams[i], sizeof(byte), 1, pFile);
			}
		}
		else
		{
			// no teams
			char t = 0;
			fwrite(&t, sizeof(bool), 1, pFile);
		}

		//write end symbol
		MapEnd end;
		end.sig[0] = 'E';
		end.sig[1] = 'N';
		end.sig[2] = 'D';

		fwrite(&end, sizeof(MapEnd), 1, pFile);
		return true;
	}
public:
	static void AdjustMapPlanet(MapPlanet* p)
	{
		p->HP =(int)float(2.7f * (powf((p->radius + 15.0f),2.0f) - powf((p->radius),2.0f))); //int(sqrtf(p->radius) * 200.0f + 1000.0f); 3.282f
		p->sUnit = 28.125f / p->radius + 0.75f;//4.0f * powf(float(M_E), logf(0.97265f) * p->radius);
	}
	std::vector< TeamID > GetTeams() const
	{
		return teams;
	}
	bool HasTeamSupport() const
	{
		return bTeamsSet;
	}
private:
	void MakeInvalid()
	{
		memset(header.sig, 0, 3);
	}
	void init(FILE* pFile)
	{
		fread(&header, sizeof(MapHeader), 1, pFile);

		if (!isValid())
		{
			return;
		}

		//Supportet version?
		if (header.Version != 2 && header.Version != 3 && header.Version != curVersion)
		{
			MakeInvalid();
			return;
		}

		if (header.isProtected)
		{
			pwd = header.passwd;
			for (auto& c : pwd)
			{
				c = ~c;
			}
		}

		MapPlanet p;
		for (int i = 0; i < header.nPlanets; i++)
		{
			fread(&p, sizeof(MapPlanet), 1, pFile);
			planets.push_back(p);
		}

		MapSpawn s;
		if (header.Version == 2)
		{
			struct oldSpawn
			{
				float x, y;
				byte team;
				int nUnits;
			} so;
			for (int i = 0; i < header.nSpawns; i++)
			{
				fread(&so, sizeof(oldSpawn), 1, pFile);
				s.nUnits = so.nUnits;
				s.team = so.team;
				s.type = E_NORMAL;
				s.x = so.x;
				s.y = so.y;
				spawns.push_back(s);
			}
		}
		else
		{
			for (int i = 0; i < header.nSpawns; i++)
			{
				fread(&s, sizeof(MapSpawn), 1, pFile);
				spawns.push_back(s);
			}
		}

		if (header.Version >= 4)
		{
			// teams supported
			fread(&bTeamsSet, sizeof(bool), 1, pFile);
			if (bTeamsSet)
			{
				teams.clear();
				for (size_t i = 0; i < header.nTeams; i++)
				{
					byte t;
					fread(&t, sizeof(byte), 1, pFile);
					teams.push_back(t);
				}
			}
			else
			{
				LoadStandartTeams();
			}
		}
		else
		{
			// standart teams
			LoadStandartTeams();
		}

		//Check ending
		MapEnd end;
		memset(&end, 0, sizeof(end));
		fread(&end, sizeof(MapEnd), 1, pFile);

		if ((end.sig[0] != 'E') || (end.sig[1] != 'N') || (end.sig[2] != 'D'))
		{
			//wrong file end??
			MakeInvalid();
		}
	}
	void LoadStandartTeams()
	{
		teams.clear();
		bTeamsSet = false;
		for (size_t i = 0; i < header.nTeams; i++)
		{
			teams.push_back(i + 1);
		}
	}
	static bool TestTeams(const std::vector< TeamID >& tms, byte maxt)
	{
		if (maxt > tms.size())
			return false;

		for (TeamID i = 0; i < maxt; i++)
		{
			if (tms[i] != i + 1)
				return true;
		}
		return false;
	}
	MapHeader header;
	std::vector< MapPlanet > planets;
	std::vector< MapSpawn > spawns;
	std::vector< TeamID > teams;
	std::string pwd;
	bool bTeamsSet = false;
};
