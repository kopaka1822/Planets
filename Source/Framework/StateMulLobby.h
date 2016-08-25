#pragma once
#include "GameState.h"
#include "UICountdownClock.h"
#include "UIPlayerList.h"
#include "../Server/Client.h"
#include "../Game/PacketTypes.h"
#include "../Game/RemoteMap.h"
#include "StateGameMulti.h"

class StateMulLobby : public GameState
{
public:
	StateMulLobby()
		:
		lblTitle("Lobby", FONT_BIG),
		btnDisconnect("Leave", FONT_SMALL),
		clock(FONT_MED),
		btnReady("Ready", FONT_SMALL),
		gs(Database::GetGameStruct())
	{
		gs.pChat = std::unique_ptr<UIChatBox>(new UIChatBox(FONT_SMALL));

		lblTitle.centerX(50.0f);

		float btnwi = 150.0f;
		btnReady.setWidth(btnwi);
		btnDisconnect.setWidth(btnwi);
		AlignObjects({ 150, 200 }, 20.0f, std::initializer_list<UIObject*>{
			&btnReady,
			&btnDisconnect });

		clock.setOrigin({ 100.0f, 50.0f });
		clock.disable();

		list.setMetrics(PointF(700, 550));
		list.setOrigin({ 450.0f, 150.0f });

		gs.pChat->setOrigin({ 25.0f, 350 });
		gs.pChat->setMetrics({ 400.0f, Framework::STD_DRAW_Y - 360 });

		AddObject(&btnDisconnect);
		AddObject(&btnReady);
		AddObject(&list);
		AddObject(gs.pChat.get());

		SendUsername();
	}
	virtual ~StateMulLobby()
	{
		if (!bGameStart)
		{
			if (gs.pClient)
				gs.pClient->SendDisconnect();
			Database::ClearGameStruct();
		}
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.draw(draw);
		btnDisconnect.draw(draw);

		if (clock.isEnabled())
			clock.draw(draw);
		btnReady.draw(draw);

		muList.Lock();
		list.draw(draw);
		muList.Unlock();

		gs.pChat->draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnDisconnect.Pushed())
		{
			gs.pClient->SendDisconnect();

			std::string  serv;
			unsigned short port;
			Database::GetMainServer(&serv, &port);
			if (serv.length())
			{
				Database::SetServerAdress(serv, port);
				SetNextState(states::Connect);
			}
			else
			{
				SetNextState(states::ServerSelect);
			}
			return;
		}

		if (gs.pChat->HasMessage())
		{
			std::string ms = gs.pChat->getMessage();
			SendChat(ms);
		}

		if (btnReady.Pushed())
		{
			SendReady();
			Settings::UnlockAchievement(Settings::A_ReadyButton);
		}

		switch (gs.pClient->GetState())
		{
		case Client::States::Connected:
			for (const auto& c : gs.pClient->GetData())
			{
				try
				{
					ContainerReader r(c);
					
					PacketType t = (PacketType)r.readShort();
					HandleData(t, r);
				}
				catch (const std::logic_error& e)
				{
					Log::Warning(std::string("invalid package: ") + e.what());
				}
			}
			gs.pClient->Update();
			break;
		case Client::States::Timeout:
			throw GameError("Connection Timed Out");
			break;
		case Client::States::Error:
			throw GameError(gs.pClient->GetError());
			break;
		default:
			throw GameError("Client undefined behaviour");
		}
	}

protected:
	void SendUsername()
	{
		std::string name = Settings::GetName();
		DataContainer con = gs.pClient->GetConRel();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::Username);
		w.writeString(name.c_str());

		gs.pClient->SendReliable(std::move(con));
	}
	virtual void HandleData(PacketType t, ContainerReader& r) = 0;
	void HandleMapPlanets(ContainerReader& r)
	{
		/*
		Message Format:
		- bool
		true : (next is planet)
		false : Packet end or no more planet
		if (true )
		float posx
		float posy
		float radius
		byte team
		int HP
		float sUnit
		*/

		while (r.readChar() == 1)
		{
			MapLoader::MapPlanet p;
			p.x = r.readFloat();
			p.y = r.readFloat();
			p.radius = r.readFloat();
			p.team = r.readByte();
			p.HP = r.readInt();
			p.sUnit = r.readFloat();

			MapPlanets.push_back(p);
		}
	}
	void HandleMapEntities(ContainerReader& r)
	{
		/*
		Message Format:
		byte team (once)

		- bool
		true : (next is entity)
		false : Packet end or no more planet
		if (true )
		float posx
		float posy
		byte type
		*/
		MapLoader::MapEntity e;
		e.team = r.readByte();

		while (r.readChar() == 1)
		{
			e.x = r.readFloat();
			e.y = r.readFloat();
			e.type = r.readByte();

			MapEntities.push_back(e);
		}
	}
	void HandleMapTeams(ContainerReader& r)
	{
		byte nPlayer = r.readByte();
		gs.ty = (Map::GameType)r.readByte();
		if (gs.ty == Map::GameType::Allicance)
		{
			teams.clear();
			for (byte i = 0; i < nPlayer; i++)
			{
				teams.push_back(r.readByte());
			}
		}
	}
	void HandleMapComplete(ContainerReader& r)
	{
		assert(MapComplete == false);
		if (MapComplete)
			return;

		MapComplete = true;

		MapWidth = r.readFloat();
		MapHeight = r.readFloat();

		// Send all ready
		SendMapRecieved();
	}
	void SendMapRecieved()
	{
		DataContainer con = gs.pClient->GetConRelSmall();
		ContainerWriter w(con);
		w.writeShort((short)PacketType::ClientAllRecieved);

		gs.pClient->SendReliable(std::move(con));
	}
	void SendReady()
	{
		if (bReady)
			return;

		DataContainer con = gs.pClient->GetConRelSmall();
		ContainerWriter w(con);
		w.writeShort((short)PacketType::ClientReady);

		gs.pClient->SendReliable(std::move(con));
		bReady = true;
	}
	void HandleGameStart(ContainerReader& r)
	{
		float start = r.readFloat();
		gs.nTeams = r.readByte();
		gs.pMap = std::unique_ptr<RemoteMap>(new RemoteMap(*gs.pClient, gs.nTeams, MapPlanets, MapEntities,
			MapWidth, MapHeight, start, gs.ty, teams));

		bGameStart = true;
		SetNextState(states::GameMulti);
	}
	void HandlePlayerList(ContainerReader& r)
	{
		struct info
		{
			byte team;
			std::string name;
			float ping;
			bool ready;
		};

		std::vector< info > data;

		const int count = r.readByte();
		for (int i = 0; i < count; i++)
		{
			info in;
			in.team = r.readByte();
			in.name = r.readString();
			in.ping = r.readFloat();
			in.ready = (r.readChar() != 0);

			data.push_back(in);
		}

		// update list
		muList.Lock();
		list.Clear();
		gs.playernames.assign(25, std::string());
		for (const auto& i : data)
		{
			list.AddItem(i.name, i.ping, i.team,i.ready);
			if (i.team > 0 && i.team < 25)
				gs.playernames[i.team - 1] = i.name;
		}
		muList.Unlock();
	}
	void HandleGameChat(ContainerReader& r)
	{
		byte team = r.readByte();

		gs.pChat->AddMessage(r.GetCurrent(), team);
	}
	void HandleForceStop(ContainerReader& r)
	{
		throw GameError(r.GetCurrent());
	}
private:
	void SendChat(const std::string& mes)
	{
		StateGameMulti::SendChat(mes, *gs.pClient);
	}
protected:
	UICountdownClock clock;
	UIButtonText btnReady;

	UIPlayerLister list;

	GameStruct& gs;

	// map data
	std::vector< MapLoader::MapPlanet > MapPlanets;
	std::vector< MapLoader::MapEntity > MapEntities;
	std::vector< byte > teams;
	float MapWidth = -1.0f;
	float MapHeight = -1.0f;
	bool MapComplete = false;
	bool bReady = false;
private:
	UILabel lblTitle;
	UIButtonText btnDisconnect;
	bool bGameStart = false;
	Mutex muList;
};
