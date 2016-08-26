#include "GameServerNormal.h"
#include "MultiGameLobby.h"
#include <memory>

GameServerNormal::GameServerNormal(PORT port, int maxPlayer, Map::GameType ty)
	:
	myPort(port),
	gameType(ty),
	chatHandler(*this)
{
	// Create Server
	pServer = std::unique_ptr<Server>(new Server(port, maxPlayer));

	PlayerInfo t;
	ResetPlayerInfo(t);
	infos.assign(maxPlayer, t);

	chatHandler.RegisterCommand("ping", std::mem_fn(&GameServerNormal::HandlePing),"shows ping");
	chatHandler.RegisterCommand("surrender", std::mem_fn(&GameServerNormal::HandleSurrender), "giving up");
	chatHandler.RegisterCommand("count", std::mem_fn(&GameServerNormal::HandleCount), "counts units");

	chatHandler.GenerateHelpMessage();
}

GameServerNormal::~GameServerNormal()
{
	Close();
}

GameServer::status GameServerNormal::UpdateGame(float dt)
{
	assert(pMap);

	// recieve data
	for (auto& c : pServer->GetData())
	{
		try
		{
			ContainerReader r(c);
			const int pid = r.readInt(); // player id
			const byte team = infos[pid - 1].team;

			if (team < 1)
				continue;

			PacketType t = (PacketType)r.readShort();

			switch (t)
			{
			case PacketType::ClientChat:
				ProcessChat(pid, r.readString());
				break;
			default:
				pMap->addPacket(std::move(c));
			}
		}
		catch (const std::logic_error&)
		{

		}
	}

	pMap->update(dt);

	if (pMap->gameEnd() != 0)
		return status::Next;

	for (const auto& e : pServer->GetEvents())
	{
		switch (e.e)
		{
		case Server::Event::TimedOut:
			Print("player " + std::to_string(e.playerID) + " timed out");
		case Server::Event::Disconnect:
			Print("player " + std::to_string(e.playerID) + " disconnected");
			infos[e.playerID - 1].bConnected = false;
			// check if all disconnected
			bool bTempRun = false;
			for (const auto& p : infos)
			{
				if (p.bConnected)
				{
					bTempRun = true;
					break;
				}
			}
			if (!bTempRun)
				return status::Abort;
			break;
		}
	}

	return status::Retry;
}

void GameServerNormal::Start()
{
	Thread::Begin();
}

bool GameServerNormal::isJoinable()
{
	if (pServer)
	{
		return pServer->isJoinable();
	}
	else
		return false;
}

bool GameServerNormal::isFinished()
{
	return !bRun;
}

void GameServerNormal::Close()
{
	if (bRun)
	{
		bRun = false;
		Thread::Join();
	}
	pMap.reset();
	pServer.reset();
}

PORT GameServerNormal::GetPort() const
{
	return myPort;
}
size_t GameServerNormal::GetModi() const
{
	switch (gameType)
	{
	case Map::GameType::AllvAll:
		return MultiGameLobby::ALLVSALL;
		break;
	case Map::GameType::Allicance:
		return MultiGameLobby::ALLIANCE;
		break;
	case Map::GameType::UnholyAlliance:
		return MultiGameLobby::UNHOLY;
		break;
	default:
		return -1;
	}
}
