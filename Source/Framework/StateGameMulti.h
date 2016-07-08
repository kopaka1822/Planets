#pragma once
#include "StateGame.h"
#include "UIChatBox.h"

class StateGameMulti : public StateGame
{
public:
	StateGameMulti()
		:
		StateGame(Database::GetGameStruct().myTeam,
		Database::GetGameStruct().nTeams)
	{
		

		assert(gs.pClient != nullptr);

		AddObject(gs.pChat.get());
		AddObject(&btnChat);

		btnPlayer.Enable();
		btnChat.Enable();

		Event_Resize(Framework::DrawStart(), Framework::DrawWidth());

		Settings::UnlockAchievement(Settings::A_Online);
	}
	virtual ~StateGameMulti()
	{
		if (gs.pClient)
		{
			gs.pClient->SendDisconnect();
		}
	}
	virtual void Event_Resize(const PointF& ul, const PointF& dim) override
	{
		StateGame::Event_Resize(ul, dim);

		gs.pChat->SetOrigin(ctrlBar.TopLeft() + PointF(10.0f,-410.0f));
		gs.pChat->SetMetrics({ 500.0f, 400.0f });
	}
	virtual void ComposeFrame(Drawing& draw) override final
	{
		DrawMap(draw);

		gs.pChat->Draw(draw);

		

		if (!gs.pMap->GameStart())
		{
			// print waiting
			FONT_SMALL.SetColor(Color::White());
			FONT_SMALL.Text("waiting...", Framework::DrawStart() + PointF(10,10));
		}
	}
	virtual void ExecuteCode(float dt) override final
	{
		assert(gs.pClient);
		// Update Server
		
		gs.pClient->Update();

		switch (gs.pClient->GetState())
		{
		case Client::States::Connected:

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

		for (auto& con : gs.pClient->GetData())
		{
			try
			{
				HandleData(con);
			}
			catch (const std::logic_error& e)
			{
				Log::Warning(std::string("invalid package: ") + e.what());
			}
		}
		

		UpdateMap(dt);
		UpdateGameButtons();

		if (btnChat.Pushed())
		{
			if (gs.pChat->GetForeground())
			{
				gs.pChat->SetForeground(false);
			}
			else
			{
				gs.pChat->SetForeground(true);
			}
		}

		

		if (gs.pChat->HasMessage())
		{
			std::string ms = gs.pChat->getMessage();
			SendChat(ms);
		}

		if (gs.pChat->HasChange())
		{
			if (!gs.pChat->GetForeground())
			{
				btnChat.SetColor(Color::Red());
			}
		}

		UpdateAlliance();

		if (gs.pChat->GetForeground())
			btnChat.SetColor(Color::White());
	}
	virtual bool BackgroundUpdate() override final
	{
		return true;
	}
	virtual void Event_GameKeyUp(Input::GameKey k, const PointF& p) override
	{
		if (!gs.pChat->isTyping() && !tablist.isVisible())
		{
			StateGame::Event_GameKeyUp(k, p);
		}
	}
	virtual void Event_GameKeyDown(Input::GameKey k, const PointF& p) override
	{
		if (!gs.pChat->isTyping() && !tablist.isVisible())
		{
			StateGame::Event_GameKeyDown(k, p);
		}
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		StateGame::Event_MouseDown(k, pos);

		if (!ctrlBar.PointInside(pos))
		{
			gs.pChat->StopTyping();
		}
	}
public:
	static void SendChat(const std::string& mes, Client& client)
	{
		DataContainer con = NetServer::GetConRel();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::ClientChat);
		w.writeString(mes.c_str());

		client.SendReliable(std::move(con));
	}
	static void HandleChat(ContainerReader& r, UIChatBox& chat)
	{
		byte team = r.readByte();
		auto txt = r.readString();

		chat.AddMessage(txt, team);
	}
private:
	void HandleData(DataContainer& con)
	{
		ContainerReader r(con);

		PacketType ty = (PacketType)r.readShort();

		switch (ty)
		{
		//case PacketType::ServerGameStart:
		//	break;
		case PacketType::GameChat:
		{
			HandleChat(r, *gs.pChat);
		}
			break;
		case PacketType::GameTeamSurrender:
			Sound::Effect(Sound::S_LOSE);
			gs.pMap->AddPacket(std::move(con));
			break;
		case PacketType::GameClanChange:
		{
			if (r.remaining() != 4)
				return;
			byte t1 = r.readByte();
			Map::ClanInfo i1 = Map::ClanInfo(r.readByte());

			byte t2 = r.readByte();
			Map::ClanInfo i2 = Map::ClanInfo(r.readByte());


			assert(t1 > 0 && t1 <= gs.nTeams);
			assert(t2 > 0 && t2 <= gs.nTeams);
			if (t1 - 1 >= gs.nTeams)
				return;
			if (t2 - 1 >= gs.nTeams)
				return;
			if (i1 > Map::ClanInfo::awaiting)
				return;
			if (i2 > Map::ClanInfo::awaiting)
				return;

			HandleClanChange(t1, i1, t2, i2);

			gs.pMap->AddPacket(std::move(con));
		}
			break;
		default:
			gs.pMap->AddPacket(std::move(con));
			break;
		}

	}
	void UpdateAlliance()
	{
		byte team = 1;
		for (auto& b : tablist.GetButtons())
		{
			if (b->Pushed())
			{
				// send request etc..
				switch (b->GetState())
				{
				case UIClanButton::state::ally:
				case UIClanButton::state::awaitingResponse:
					// break alliance
					SendAllyBreak(team);
					break;
				case UIClanButton::state::noAlly:
				case UIClanButton::state::sendResponse:
					// send request
					SendAllyRequest(team);
					break;
				}
			}
			team++;
		}
	}
	void SendChat(const std::string& mes)
	{
		SendChat(mes, *gs.pClient);
	}
	void SendAllyRequest(byte team)
	{
		DataContainer con = gs.pClient->GetConRelSmall();
		ContainerWriter w(con);
		w.writeShort((short)PacketType::ClientClanRequest);
		w.writeByte(team);

		gs.pClient->SendReliable(std::move(con));
	}
	void SendAllyBreak(byte team)
	{
		DataContainer con = gs.pClient->GetConRelSmall();
		ContainerWriter w(con);
		w.writeShort((short)PacketType::ClientClanDestroy);
		w.writeByte(team);

		gs.pClient->SendReliable(std::move(con));
	}

	void HandleClanChange(byte t1, Map::ClanInfo i1, byte t2, Map::ClanInfo i2)
	{
		assert(t1 > 0);
		assert(t2 > 0);
		if (t1 == 0 || t2 == 0 || t1 > gs.nTeams || t2 > gs.nTeams)
			return;

		if (myTeam != t1)
		{
			std::swap(t1, t2);
			std::swap(i1, i2);
		}

		if (myTeam != t1)
			return; // doesnt concern me
		
		std::vector<UIClanButton*>& btns = tablist.GetButtons();
		std::string plname = gs.playernames[t2 - 1];
		UIClanButton* otherButton = btns[t2 - 1];

		if (i1 == Map::ClanInfo::Ally)
		{
			// ally both
			if (otherButton->GetState() != UIClanButton::state::ally)
			{
				gs.pChat->AddMessage(plname + " is now your ally", t2);
			}
			otherButton->SetState(UIClanButton::state::ally);
		}
		else if (i1 == Map::ClanInfo::noAlly)
		{
			// unally
			if (otherButton->GetState() == UIClanButton::state::ally)
			{
				gs.pChat->AddMessage(plname + " is no longer your ally", t2);
			}
			otherButton->SetState(UIClanButton::state::noAlly);
		}
		else if (i1 == Map::ClanInfo::awaiting)
		{
			// im awaiting his answer
			otherButton->SetState(UIClanButton::state::awaitingResponse);
		}
		else if (i1 == Map::ClanInfo::sendRequest)
		{
			// he send me a request
			if (otherButton->GetState() != UIClanButton::state::sendResponse)
			{
				gs.pChat->AddMessage(plname + " wants to form an alliance", t2);
			}
			otherButton->SetState(UIClanButton::state::sendResponse);
		}
	}
};