#include "GameServerNormalFixedMap.h"

GameServerNormalFixed::GameServerNormalFixed(const MapLoader& l, PORT port, Map::GameType ty)
	:
	GameServerNormal(port,l.GetPlayers(),ty)
{
	// map can be created here
	LoadMap(l);
}
GameServerNormalFixed::~GameServerNormalFixed()
{

}

GameServer::status GameServerNormalFixed::UpdateLobby(float dt)
{
	UpdateLobbyEvents();

	bool bResendList = false;

	for (const auto& c : pServer->GetData())
	{
		ContainerReader r(c);
		int id = r.readInt();
		PacketType t = (PacketType)r.readShort();

		switch (t)
		{
		case PacketType::Username:
			HandleUsername(id, r);
			SendMap(id);
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
			if (r.remaining() < 2)
				continue;
			ProcessChat(id, r.GetCurrent());
			break;

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

	// Server full ?
	if (AllPlayersReady())
	{
		Print("preparing to start");
		SendGameStart(-3.0f, pServer->GetMaxPlayers());
		pMap->SetStartClock(-3.0f);
		return status::Next;
	}
	return status::Retry;
}

void GameServerNormalFixed::HandleLobbyJoin(int id)
{
	SendServerType(id);
	for (auto& c : pMap->GetStartData())
	{
		pServer->SendContainerReliable(std::move(c), id);
	}
}