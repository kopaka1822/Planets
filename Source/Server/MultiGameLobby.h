#pragma once
#include "../Game/MapLoader.h"
#include <map>
#include "Server.h"
#include "GameServer.h"
#include <memory>
#include "../Game/PacketTypes.h"
#include "CommmandHandler.h"
#include <time.h>
#include "../Utility/Exception.h"

class MultiGameLobby
{
public:
	enum GameModi
	{
		ALLVSALL,
		UNHOLY,
		ALLIANCE
	};
	enum PAGES // add pages here
	{
		STARTPAGE
	};
	enum PageID
	{
		BTN_ALLVALL,
		BTN_ALLIANCE,
		BTN_UNHOLY
	};
	struct PageItem
	{
		PageItem(){}
		PageItem(PageObject ty, int i, std::string tx)
			:
			type(ty),
			id((PageID)i),
			text(tx)
		{}
		PageObject type;
		PageID id;
		std::string text;
	};
	struct PageColor
	{
		PageColor(byte r, byte g, byte b)
			:
			a(255), r(r), g(g), b(b){}
		union
		{
			int color;
			struct
			{
				byte b, g, r, a;
			};
		};
	};
	enum LIST
	{
		LEFT,
		RIGHT
	};
private:
	struct PlayerInfo
	{
		bool bConnected;
		std::string name;
	};
public:
	MultiGameLobby();
	~MultiGameLobby();
	void LoadSettings();
	void Init(int maxPlayerLobby);

	void Update(float dt);

	const MapLoader& GetMap(unsigned int nPlayer) const
	{
		auto e = mapsOrdered.find(nPlayer);
		if (e == mapsOrdered.end())
			throw Exception("get map invalid call");

		int nmaps = e->second.size();
		return e->second[rand() % nmaps];
	}

	const MapLoader& GetMapTeam(unsigned int nPlayer) const
	{
		auto e = mapsTeams.find(nPlayer);
		assert(e != mapsTeams.end());
		if (e == mapsTeams.end())
			throw Exception("get team map invalid call");

		int nmaps = e->second.size();
		return e->second[rand() % nmaps];
	}
	int GetMaxTeamPlayer() const
	{
		return maxTeamSize;
	}
	int GetMinTeamPlayer() const
	{
		return minTeamSize;
	}
	int GetMaxPlayerSize() const
	{
		return maxPlayerSize;
	}
	void ReloadMaps();
public:
	// chat callback
	void HandlePing(int id, std::list< std::string > args);
private:
	void SendServerMessage(int id, const char* txt, byte color = 0);
	void LoadMaps();
	void OrderMaps();
	bool LoadFromFile();
	void CreateSaveFile();

	void RedirectPlayer(int id, size_t modi);
	void SendServerInfo(int id);

	PORT GetFreeGamePort(size_t modi);
	void ResetPlayerInfo(PlayerInfo* i)
	{
		i->bConnected = false;
		i->name.clear();
	}

	void SendStartPage(int id);
	void SendPageHeadline(PAGES id, const std::string& txt, int player);
	void SendPageList(const std::vector< PageItem >& list, int player, LIST l);
	void HandleButton(ContainerReader& r, int player);
	void HandleChat(ContainerReader& r, int id);
	void UpdateListWithPlayerPings(LIST side);
	void CloseGames();
private:
	std::unique_ptr<Server> pServer;
	int maxPlayerSize = 0;
	int maxTeamSize = 0;
	int minTeamSize = 0;
	int maxPackageSize = 0;
	std::vector< MapLoader > maps;
	std::map< unsigned int, std::vector< MapLoader >> mapsOrdered;
	std::map< unsigned int, std::vector< MapLoader >> mapsTeams;

	int maxGames = 0;
	std::vector< std::unique_ptr<GameServer> > servers;
	std::vector< PlayerInfo > players;

	// Interface
	std::vector< PageItem > startPageLeft;

	float listTime = 0.0f;

	CommandHandler<MultiGameLobby> chatHandler;
};
