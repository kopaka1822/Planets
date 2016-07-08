#pragma once
#include "GameState.h"
#include "UIWaiting.h"
#include "../Server/Client.h"
#include "../Game/PacketTypes.h"
#include "../Game/Version.h"

class StateConnect : public GameState
{
public:
	StateConnect()
		:
		lblTitle("Connecting",FONT_BIG),
		progWaiting(Color::White()),
		btnAbort("Cancel",FONT_MED),
		lblInfo("...",FONT_SMALL),
		gs(Database::ClearGameStruct())
	{
		lblTitle.CenterX(50.0f);
		lblInfo.CenterX(600.0f);
		progWaiting.SetMetrics(PointF(300.0f, 300.0f));
		progWaiting.CenterX(200.0f);

		btnAbort.AdjustToFont();
		btnAbort.SetOrigin(PointF(10.0f, Framework::STD_DRAW_Y - 10.0f - btnAbort.GetMetrics().y));

		AddObject(&btnAbort);
		Database::GetServerAdress(&servIP, &servPort);

	}
	virtual ~StateConnect()
	{
		if (!bKeepClient)
			Database::ClearGameStruct(); // this will delete the client
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.Draw(draw);
		if (!denied)
			progWaiting.Draw(draw);
		btnAbort.Draw(draw);
		lblInfo.Draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		progWaiting.Update(dt);

		if (btnAbort.Pushed())
		{
			SetNextState(states::ServerSelect);
		}

		if (gs.pClient == nullptr)
		{
			// make client and handle firewall
			gs.pClient = std::unique_ptr<Client>(new Client());

			gs.pClient->Connect(servIP, servPort);
		}
		else if (!denied)
		{
			// update Client
			UpdateClient();
		}
	}
private:
	void UpdateClient()
	{
		gs.pClient->Update();

		switch (gs.pClient->GetState())
		{
		case Client::States::notConnected:
			SetInfo("Searching...");
			break;
		case Client::States::Connecting:
			SetInfo("Sending Request");
			break;
		case Client::States::Connected:
			SetInfo("Connected");
			HandleData();
			break;
		case Client::States::Denied:
			SetInfo(gs.pClient->GetError());
			lblTitle.SetText("Denied");
			lblTitle.AdjustToFont();
			lblTitle.CenterX(50.0f);
			denied = true;
			break;
		case Client::States::Disconnected:
			SetInfo("Disconnected");
			denied = true;
			break;
		case Client::States::Error:
			denied = true;
			throw GameError(gs.pClient->GetError());
			break;
		case Client::States::Timeout:
			denied = true;
			throw GameError("Connection Timed out");
			break;
		default:
			break;
		}
	}
	void SetInfo(const std::string& s)
	{
		lblInfo.SetText(s);
		lblInfo.AdjustToFont();
		lblInfo.CenterX(600.0f);
	}
	void HandleData()
	{
		for (auto& c : gs.pClient->GetData())
		{
			try
			{
				ContainerReader r(c);
				PacketType t = (PacketType)r.readShort();
				switch (t)
				{
				case PacketType::ServerType:
					HandleServerInfo(r);
					return;
					break;
				}
			}
			catch (const std::logic_error& e)
			{
				throw GameError(std::string("invalid package: ") + e.what());
			}
		}
	}
	void HandleServerInfo(ContainerReader& r)
	{
		int version = r.readInt();
		if (!Version::IsSupported(version))
			throw GameError("Server Version not supported: " + std::to_string(Version::GetGameVersion()));

		const ServerTypes type = (ServerTypes)r.readByte();
		

		if (type == ServerTypes::OneLobby)
		{
			gs.myTeam = r.readByte();
			gs.nTeams = r.readByte();

			bKeepClient = true;
			SetNextState(states::MultiLobbyFixed);
		}
		else if (type == ServerTypes::Redirecting)
		{
			PORT rePort = r.readShort();
			// redirect to this port
			gs.pClient->SendDisconnect();

			Database::SetMainServer(servIP, servPort);
			gs.pClient.reset();

			if (rePort == 0)
				throw GameError("no free slots available");

			gs.pClient = std::unique_ptr<Client>(new Client());
			gs.pClient->Connect(servIP, rePort);

			return;
		}
		else if (type == ServerTypes::DynClient)
		{
			gs.myTeam = r.readByte();
			bKeepClient = true;
			SetNextState(states::MultiLobbyDyn);
		}
		else if (type == ServerTypes::GameSelectLobby)
		{
			bKeepClient = true;
			Database::SetMainServer(servIP, servPort);
			SetNextState(states::ServerStartPage);
		}
		else
		{
			throw GameError("Incompatible Server Type");
		}
	}
private:
	UILabel lblTitle;
	UILabel lblInfo;
	UIWaiting progWaiting;
	UIButtonText btnAbort;
	GameStruct& gs;

	std::string servIP;
	unsigned short servPort;

	bool denied = false;
	bool bKeepClient = false;

	int tries = 0; // reconnect tries
};
