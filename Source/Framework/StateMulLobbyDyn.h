#pragma once
#include "StateMulLobby.h"

class StateMulLobbyDyn : public StateMulLobby
{
public:
	StateMulLobbyDyn()
		:
		StateMulLobby()
	{}
	virtual ~StateMulLobbyDyn()
	{}

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
		case PacketType::LobbyTime:
			HandleLobbyTime(r);
			break;
		case PacketType::LobbyTeamAssign:
			HandleLobbyAssign(r);
			break;
		case PacketType::GameChat:
			HandleGameChat(r);
			break;
		case PacketType::GameForceStop:
			HandleForceStop(r);
			break;
		}
	}

	void HandleLobbyTime(ContainerReader& r)
	{
		float time = r.readFloat();
		clock.SetTimer(time);
		clock.Enable();
	}
	void HandleLobbyAssign(ContainerReader& r)
	{
		gs.myTeam = r.readByte();
	}
};