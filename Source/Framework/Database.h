#pragma once
#include <string>
#include <vector>
#include "../Game/Map.h"
#include "../Game/Levelpack.h"

using byte = unsigned char;

struct GameResults
{
	bool isWinner;

	struct stats
	{
		unsigned int entSpawn;
		unsigned int entDie;
		unsigned int planCap;
		unsigned int planLost;
	};

	std::vector< stats > data;

	std::vector< std::vector< unsigned int >> unitCounts;
	float dtUnitCount;
};

struct GameStruct
{
	byte myTeam;
	byte nTeams;
	std::unique_ptr<Map> pMap;
	std::unique_ptr<class Client> pClient;
	std::vector< std::string > playernames;
	std::unique_ptr<class UIChatBox> pChat;
	Map::GameType ty;
	float time;

	bool lvlPackLevel;
};

struct LevelpackStruct
{
	int curLevelpack;
	int curLvl;

};

class Database
{
public:
	enum GameTex
	{
		Particle,
		ParticleExpl,
		ParticleTank,
		ParticleSabo,
		Logo,
		StoryPart,
		StoryBomb,
		StorySabo,
		StorySpeed,

		StarEmpty,
		StartFilled,

		Shield,

		zoom,
		drag,
		swipe,
		selector,

		StarField,
		// add here

		SIZE
	};
public:
	static bool Initialize(class Drawing& draw);
	static void Shutdown();

	static class Texture& GetTexture(GameTex tex);

	static void SetMap(class Map* pMap);
	static class Map* GetMap();

	static void SetResults(GameResults r);
	static const GameResults& GetResults();

	static void SetServerAdress(const std::string& ip, unsigned short port);
	static void GetServerAdress(std::string*, unsigned short*);

	static void SetMainServer(const std::string& ip, unsigned short port);
	static void GetMainServer(std::string* ip, unsigned short* port);
	static bool HasMainServer();

	// Editor
	static void SetEditorTeams(const std::vector< byte >& teams);
	static const std::vector< byte >& GetEditorTeams();

	static GameStruct& GetGameStruct();
	static GameStruct& ClearGameStruct();

	static LevelpackStruct& GetLevelpackStuct();

	static void LoadLevelpacks();
	static std::vector< Levelpack >& GetLevelpacks();
};