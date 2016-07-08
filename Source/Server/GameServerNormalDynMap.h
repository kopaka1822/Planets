#pragma once
#include "GameServerNormal.h"
#include "MultiGameLobby.h"
#include "../Game/Version.h"

class GameServerNormalDynMap : public GameServerNormal
{
public:
	GameServerNormalDynMap(MultiGameLobby& gameLobby, int minpl,
		int maxpl, PORT port, Map::GameType ty);
	virtual ~GameServerNormalDynMap();

protected:
	virtual status UpdateLobby(float dt) override;
	virtual void HandleLobbyJoin(int id) override;
	virtual void Print(const std::string& s) override;
private:
	bool DeliverMap();
	void SendServerInfo(int id)
	{
		DataContainer con = pServer->GetConRelSmall();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::ServerType);
		w.writeInt(Version::GetGameVersion());
		w.writeByte((char)ServerTypes::DynClient);
		w.writeByte((char)id);

		pServer->SendContainerReliable(std::move(con), id);
	}
private:
	const PORT port;
	MultiGameLobby& gameLobby;
	const int minPlayer;
	const int maxPlayer;

	bool waitingPhase = false; // phase with counter
	Timer waitTimer;

	bool bMapDelivered = false;
	int nPlayer = -1;

	float dtList = 0.0f;
	float dtLobTimer = 0.0f;
};