#pragma once
#include "StateMulLobby.h"

class StateMulLobbyFixed : public StateMulLobby
{
public:
	StateMulLobbyFixed()
		:
		StateMulLobby()
	{}
	virtual ~StateMulLobbyFixed()
	{

	}
protected:
	virtual void HandleData(PacketType t, ContainerReader& r) override
	{
		switch (t)
		{
		case PacketType::LobbyPlayerlist:
			HandlePlayerList(r);
			break;
		case PacketType::MapPlanets:
			HandleMapPlanets(r);
			break;
		case PacketType::MapEntities:
			HandleMapEntities(r);
			break;
		case PacketType::MapTeams:
			HandleMapTeams(r);
			break;
		case PacketType::MapComplete:
			HandleMapComplete(r);
			break;
		case PacketType::ServerGameStart:
			HandleGameStart(r);
			break;
		case PacketType::GameChat:
			HandleGameChat(r);
			break;
		}
	}
};