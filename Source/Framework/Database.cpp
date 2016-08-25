#include "Database.h"
#include <map>
#include "Texture.h"
#include "Drawing.h"
#include <assert.h>
#include "UIChatBox.h"
#include "../Server/Client.h"
#include "../Utility/Directory.h"
#include "../Game/Levelpack.h"

//variables
static bool bInit = false;
static std::vector< Texture* > textures; //textures will be deleted by Drawing not Database!
static GameResults results;
static std::string servAddr;
static unsigned short servPort;
static std::string mainAddr;
static unsigned short mainPort;
static std::vector< byte > editTeams;

static GameStruct gs;

static std::vector< Levelpack > levelpacks;
static LevelpackStruct levelstruct;

bool Database::Initialize(Drawing& draw)
{
	
	Log::Write("loading textures");
	textures.assign(Database::SIZE, nullptr);
	//add textures
	//textures[GameTex::Team1] = draw.MakeTexture(L"data//t1.bmp");
	textures[GameTex::Particle] = draw.MakeTexture("data//pic//particle.bmp");
	textures[GameTex::ParticleExpl] = draw.MakeTexture("data//pic//particle1.bmp");
	textures[GameTex::ParticleTank] = draw.MakeTexture("data//pic//particle2.bmp");
	textures[GameTex::ParticleSabo] = draw.MakeTexture("data//pic//particle3.bmp");
	textures[GameTex::Logo] = draw.MakeTexture("data//pic//logo.png");

	textures[GameTex::StoryPart] = draw.MakeTexture("data//pic//particle_plain.bmp");
	textures[GameTex::StoryBomb] = draw.MakeTexture("data//pic//particle1_plain.bmp");
	textures[GameTex::StorySpeed] = draw.MakeTexture("data//pic//particle2_plain.bmp");
	textures[GameTex::StorySabo] = draw.MakeTexture("data//pic//particle3_plain.bmp");

	textures[GameTex::StarEmpty] = draw.MakeTexture("data//pic//star_e.png");
	textures[GameTex::StartFilled] = draw.MakeTexture("data//pic//star_f.png");

	textures[GameTex::Shield] = draw.MakeTexture("data//pic//shield.bmp");

	textures[GameTex::zoom] = draw.MakeTexture("data//pic//zoom.bmp");
	textures[GameTex::drag] = draw.MakeTexture("data//pic//drag.bmp");
	textures[GameTex::swipe] = draw.MakeTexture("data//pic//swipe.bmp");
	textures[GameTex::selector] = draw.MakeTexture("data//pic//selector.bmp");

	textures[GameTex::StarField] = draw.MakeTexture("data//pic//star.bmp");
	textures[GameTex::Play] = draw.MakeTexture("data//pic//play.bmp");
	textures[GameTex::FastForward] = draw.MakeTexture("data//pic//fastforward.bmp");

	levelstruct.curLevelpack = 0;
	levelstruct.curLvl = 0;

	bInit = true;
	return true;
}

LevelpackStruct& Database::GetLevelpackStuct()
{
	return levelstruct;
}

void Database::LoadLevelpacks()
{
	Log::Write("loading levelpacks");

	const std::string path = "data/levelpacks/";
	auto files = GetDirectoryFilenames("data/levelpacks");

	// iterate through all files
	levelpacks.clear();
	for (const auto& name : files)
	{
		if (!tool::fileHasExtension(name, ".pack"))
			continue;

		// Create Levelpack
		Levelpack pack(path + name);

		if (pack.isValid())
		{
			levelpacks.push_back(std::move(pack));
		}
		else
		{
			Log::Warning("levelpack: " + name + "seems to be invalid");
		}
	}

	Log::Write("found " + std::to_string(levelpacks.size()) + " levepacks");
}
std::vector< Levelpack >& Database::GetLevelpacks()
{
	return levelpacks;
}

Texture& Database::GetTexture(GameTex tex)
{
	assert(bInit);
	assert(tex < GameTex::SIZE);
	return *textures[tex];
}

void Database::Shutdown()
{
	ClearGameStruct();
}

void Database::SetResults(GameResults r)
{
	results = r;
}
const GameResults& Database::GetResults()
{
	return results;
}

void Database::SetServerAdress(const std::string& ip, unsigned short port)
{
	servAddr = ip;
	servPort = port;
}
void Database::GetServerAdress(std::string* s, unsigned short* p)
{
	if (s)
		*s = servAddr;

	if (p)
		*p = servPort;
}
void Database::SetMainServer(const std::string& ip, unsigned short port)
{
	mainAddr = ip;
	mainPort = port;
}
void Database::GetMainServer(std::string* ip, unsigned short* port)
{
	if (ip)
		*ip = mainAddr;
	if (port)
		*port = mainPort;
}
bool Database::HasMainServer()
{
	return mainAddr.length() != 0;
}
GameStruct& Database::GetGameStruct()
{
	return gs;
}
GameStruct& Database::ClearGameStruct()
{
	gs.myTeam = 0;
	gs.nTeams = 0;
	if (gs.pClient)
	{
		gs.pClient->SendDisconnect();
		gs.pClient.reset();
	}
	gs.pMap.reset();
	gs.playernames.clear();
	gs.pChat.reset();
	gs.ty = Map::GameType::NONE;
	gs.lvlPackLevel = false;
	gs.time = 0.0f;

	return gs;
}

void Database::SetEditorTeams(const std::vector< byte >& teams)
{
	editTeams = teams;
}
const std::vector< byte >& Database::GetEditorTeams()
{
	return editTeams;
}