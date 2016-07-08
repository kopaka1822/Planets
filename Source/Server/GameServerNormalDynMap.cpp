#include "GameServerNormalDynMap.h"
#include "ServerOutput.h"

GameServerNormalDynMap::GameServerNormalDynMap(MultiGameLobby& gameLobby, int minpl,
	int maxpl, PORT port, Map::GameType ty)
	:
	GameServerNormal(port,maxpl,ty),
	maxPlayer(maxpl),
	minPlayer(minpl),
	gameLobby(gameLobby),
	port(port)
{

}
GameServerNormalDynMap::~GameServerNormalDynMap()
{

}
void GameServerNormalDynMap::Print(const std::string& s)
{
	GlobalPrint(port, s.c_str());
}
GameServer::status GameServerNormalDynMap::UpdateLobby(float dt)
{
	UpdateLobbyEvents();

	bool bResendList = false;

	for (const auto& c : pServer->GetData())
	{
		try
		{
			ContainerReader r(c);
			int id = r.readInt();
			PacketType t = (PacketType)r.readShort();

			switch (t)
			{
			case PacketType::Username:
				HandleUsername(id, r);
				bResendList = true;
				break;
			case PacketType::ClientAllRecieved:
				HandleClientAllRecieved(id);
				break;
			case PacketType::ClientReady:
				HandleClientReady(id);
				bResendList = true;
				break;
			case PacketType::ClientChat:
				ProcessChat(id, r.GetCurrent());
				break;

			}
		}
		catch (const std::logic_error&)
		{

		}
	}

	const float WAIT_TIME = 40.0f;
	if (waitingPhase)
	{
		if (waitTimer.GetTimeSecond() > WAIT_TIME - 10.0f)
		{
			if (!bMapDelivered)
			{
				if (!DeliverMap())
				{
					SendGameForceStop("not enough players");
					Print("not enough players to start");

					return status::Abort;
				}
				bResendList = true;
				bMapDelivered = true;
				SendTeamAssign();
			}
		}
	}

	dtList += dt;
	if (dtList > 2.0f)
	{
		dtList = 0.0f;
		bResendList = true;
	}

	if (bResendList)
		SendPlayerList();

	if (waitingPhase)
	{
		dtLobTimer += dt;
		if (dtLobTimer > 2.0f)
		{
			dtLobTimer = 0.0f;
			SendLobbyTime(std::max(WAIT_TIME - waitTimer.GetTimeSecond(), 0.0f));
		}

		bool bStart = false;
		if (waitTimer.GetTimeSecond() >= WAIT_TIME && bMapDelivered)
			bStart = true; // force start

		if (!bStart && bMapDelivered && ConnectedPlayersReady())
			bStart = true;

		if (bStart)
		{
			SendGameStart(-5.0f, nPlayer);
			pMap->SetStartClock(-5.0f);
			Print("starting game");
			return status::Next;
		}
	}
	return status::Retry;
}

void GameServerNormalDynMap::HandleLobbyJoin(int id)
{
	if (pServer->GetNumConnected() >= minPlayer)
	{
		if (!waitingPhase)
		{
			waitingPhase = true;
			waitTimer.StartWatch();
			SendLobbyTime(40.0f);
		}
	}

	SendServerInfo(id);
}

bool GameServerNormalDynMap::DeliverMap()
{
	pServer->SetJoinable(false);

	nPlayer = pServer->GetNumConnected();
	if (nPlayer < minPlayer)
		return false;

	if (gameType == Map::GameType::Allicance)
		LoadMap(gameLobby.GetMapTeam(nPlayer));
	else
		LoadMap(gameLobby.GetMap(nPlayer));

	for (auto& c : pMap->GetStartData())
	{
		pServer->SendContainerAllReliable(std::move(c));
	}

	ReorderPlayers(nPlayer);
	return true;
}
