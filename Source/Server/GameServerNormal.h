#pragma once
#include "GameServer.h"
#include "../Game/PacketTypes.h"
#include "CommmandHandler.h"
#include "../Game/LocalMapServer.h"

class GameServerNormal : public GameServer
{
	struct PlayerInfo
	{
		bool bConnected;
		bool bHasMap;
		std::string name;
		byte team;
		bool bReady;
	};
public:
	GameServerNormal(PORT port, int maxPlayer, Map::GameType ty);
	virtual ~GameServerNormal();

	virtual void Start() override;
	virtual bool isFinished() override;
	virtual bool isJoinable() override;
	virtual void Close() override;
	virtual PORT GetPort() const override;
	virtual size_t GetModi() const override;
protected:
	virtual status UpdateGame(float dt) override;


	void ResetPlayerInfo(PlayerInfo& p)
	{
		p.bConnected = false;
		p.bHasMap = false;
		p.name.clear();
		p.team = 0;
		p.bReady = false;
	}
	
	void SendPlayerList()
	{
		int numCon = 0;
		for (const auto& p : infos)
			if (p.bConnected)
				numCon++;

		if (numCon == 0)
			return;

		DataContainer con = pServer->GetConRel();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::LobbyPlayerlist);
		// number of connected players
		w.writeByte(numCon);

		int id = 1;
		for (const auto& p : infos)
		{
			if (p.bConnected)
			{
				w.writeByte(p.team);
				w.writeString(p.name.c_str());
				w.writeFloat(pServer->GetPing(id));
				w.writeChar(p.bReady);
			}
			id++;
		}

		pServer->SendContainerAllReliable(std::move(con));
	}
	bool ConnectedPlayersReady() const
	{
		bool bStart = true;
		for (const auto& p : infos)
			if (p.bConnected)
				if (!p.bReady)
					bStart = false;
		return bStart;
	}
	bool AllPlayersReady() const
	{
		for (const auto& p : infos)
			if (!(p.bConnected && p.bHasMap && p.bReady))
				return false;
		return true;
	}
	void ReorderPlayers(int nPlayer)
	{
		for (int i = 1; i < nPlayer + 1; i++)
		{
			bool okay = false;
			// check if player exists
			for (const auto& p : infos)
			{
				if (p.bConnected)
				{
					if (p.team == i)
					{
						okay = true;
						break;
					}
				}
			}

			if (okay)
				continue;

			// team does not exist
			for (auto& p : infos)
			{
				if (p.bConnected)
				{
					if (p.team == 0 || p.team > nPlayer)
					{
						p.team = (byte)i;
						break;
					}
				}
			}
		}
	}

	// set joinable has to be false
	void LoadMap(const MapLoader& l)
	{
		std::function<byte(int)> GetPlayerTeam = [this](int id){return infos[id - 1].team; };

		pMap = std::unique_ptr< LocalMapServer >(new LocalMapServer(l.GetPlayers(), l.GetPlanets(),
			l.GetSpawns(), *pServer, l.GetWidth(), l.GetHeight(),
			GetPlayerTeam, gameType, l.GetTeams()));
	}

	void SendGameStart(float time, int nPlayer)
	{
		DataContainer con = pServer->GetConRelSmall();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::ServerGameStart);
		w.writeFloat(time); // time seconds until game starts
		w.writeByte(nPlayer);

		pServer->SendContainerAllReliable(std::move(con));
	}
	
	void SendTeamAssign()
	{
		int id = 1;
		for (const auto& p : infos)
		{
			if (p.bConnected)
			{
				DataContainer c = pServer->GetConRelSmall();
				ContainerWriter w(c);

				w.writeShort((short)PacketType::LobbyTeamAssign);
				w.writeByte(p.team);

				pServer->SendContainerReliable(std::move(c), id);
			}
			id++;
		}
	}
	void SendLobbyTime(float time)
	{
		DataContainer con = pServer->GetConUnrelSmall();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::LobbyTime);
		w.writeFloat(time);

		pServer->SendContainerAllUnreliable(std::move(con));
	}
public:
	void ProcessChat(int id, std::string msg)
	{
		if (msg.at(0) == '/')
		{
			if (!chatHandler.InpterpretCString(id, msg.c_str() + 1))
			{
				SendServerMessage(id, chatHandler.GetHelpMessage().c_str());
			}
		}
		else
		{
			// TODO check for max string length
			DataContainer con = pServer->GetConRel();
			ContainerWriter w(con);

			w.writeShort((short)PacketType::GameChat);

			w.writeByte(infos[id - 1].team);
			w.writeStringNoNull("[");
			w.writeStringNoNull(infos[id - 1].name.c_str());
			w.writeStringNoNull("] ");
			w.writeString(msg);

			pServer->SendContainerAllReliable(std::move(con));
		}
	}
	void SendGameForceStop(const char* msg)
	{
		pServer->SetJoinable(false);

		DataContainer c = pServer->GetConRelSmall();
		ContainerWriter w(c);

		w.writeShort((short)PacketType::GameForceStop);
		w.writeString(msg);

		pServer->SendContainerAllReliable(std::move(c));
	}

public:
	void HandlePing(int id, std::list< std::string > args)
	{
		float ping = pServer->GetPing(id);
		std::string msg = "ping: " + std::to_string((int)ping);
		SendServerMessage(id, msg.c_str());
	}
	void HandleSurrender(int id, std::list< std::string > args)
	{
		// HandleSurrender infos[id - 1] team
		// chat -> Team %d surrenderd
		if (pMap && pMap->GameStart())
		{
			pMap->HandleSurrender(infos[id - 1].team);
			SendServerMessage(0, (std::string("Team ") + std::to_string(infos[id - 1].team) + std::string(" surrendered")).c_str(), infos[id - 1].team);
		}
	}
	void HandleCount(int id, std::list< std::string > args)
	{
		if (args.size() && pMap)
		{
			switch (args.front()[0])
			{
			case 'm':
				SendServerMessage(id, std::to_string(pMap->CountUnits(infos[id - 1].team)).c_str());
				return;
			case 'a':
			{
				size_t sum = 0;
				for (byte i = 1; i <= pMap->GetNPlayers(); i++)
				{
					sum += pMap->CountUnits(i);
				}
				SendServerMessage(id, std::to_string(sum).c_str());
				return;
			}
			case 'o':
				args.pop_front();
				if (args.size())
				{
					int t = atoi(args.front().c_str());

					SendServerMessage(id, std::to_string(pMap->CountUnits(t)).c_str());
					return;
				}
			}
		}

		// send help
		SendServerMessage(id, "count:\nm - own units\na - all units\no [number] - units of team [number]");
	}
protected:
	void SendServerMessage(int id, const char* txt, byte color = 0)
	{
		DataContainer con = pServer->GetConRel();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::GameChat);
		w.writeByte(color);
		w.writeStringNoNull("[Server] ");
		w.writeString(txt);

		if (id)
			pServer->SendContainerReliable(std::move(con), id);
		else
			pServer->SendContainerAllReliable(std::move(con));
	}
	void UpdateLobbyEvents()
	{
		for (const auto& e : pServer->GetEvents())
		{
			switch (e.e)
			{
			case Server::Event::Join:
				infos[e.playerID - 1].bConnected = true;
				infos[e.playerID - 1].team = (byte)e.playerID;
				HandleLobbyJoin(e.playerID);
				break;
			case Server::Event::Disconnect:
				Print("player " + std::to_string(e.playerID) + " disconnected");
				ResetPlayerInfo(infos[e.playerID - 1]);
				break;
			case Server::Event::TimedOut:
				ResetPlayerInfo(infos[e.playerID - 1]);
				Print("player " + std::to_string(e.playerID) + " timed out");
				break;
			}
		}
	}
	virtual void HandleLobbyJoin(int id) = 0;
	void HandleUsername(int id, ContainerReader& r)
	{
		infos[id - 1].name = r.readString();
		Print(std::string("Username of ")
			+ std::to_string(id) + std::string(": ") + infos[id - 1].name);
	}
	void HandleClientReady(int id)
	{
		infos[id - 1].bReady = true;
	}
	void HandleClientAllRecieved(int id)
	{
		infos[id - 1].bHasMap = true;
	}
protected:
	const Map::GameType gameType;
	std::unique_ptr<LocalMapServer> pMap;
private:
	const PORT myPort;

	std::vector< PlayerInfo > infos;
	CommandHandler<GameServerNormal> chatHandler;
};