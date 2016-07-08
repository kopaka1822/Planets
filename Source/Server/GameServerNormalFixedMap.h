#pragma once
#include "GameServerNormal.h"
#include "../Game/Version.h"

class GameServerNormalFixed : public GameServerNormal
{
public:
	GameServerNormalFixed(const MapLoader& l, PORT port, Map::GameType ty);
	virtual ~GameServerNormalFixed();

protected:
	virtual status UpdateLobby(float dt) override;
	virtual void HandleLobbyJoin(int id) override;

private:
	void SendServerType(int playerID)
	{
		DataContainer con = pServer->GetConRelSmall();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::ServerType);
		w.writeInt(Version::GetGameVersion());
		w.writeByte((char)ServerTypes::OneLobby);
		w.writeByte(playerID);
		w.writeByte(pServer->GetMaxPlayers());

		pServer->SendContainerReliable(std::move(con), playerID);
	}
	void SendMap(int playerID)
	{
		for (auto& c : pMap->GetStartData())
			pServer->SendContainerReliable(std::move(c), playerID);
	}
private:
	float dtList = 0.0f;
};