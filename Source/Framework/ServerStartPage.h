#pragma once
#include "GameState.h"
#include "Database.h"
#include "../Game/PacketTypes.h"
#include "UIItemListerScroll.h"
#include "UIButtonTextServer.h"
#include "UITextBox.h"
#include "StateGameMulti.h"

class ServerStartPage : public GameState
{
public:
	ServerStartPage(GameStruct& gs)
		:
		lblTitle("Connecting...",FONT_BIG),
		btnDisconnect("Disconnect",FONT_MED),
		gs(gs),
		listLeft(false),
		listRight(false)
	{
		Sound::Theme(Sound::ThemeMax());

		lblTitle.centerX(50.0f);
		assert(gs.pClient);

		btnDisconnect.setOrigin(PointF(10.0f,
			Framework::STD_DRAW_Y - 10.0f - btnDisconnect.getMetrics().y));

		btnDisconnect.registerMe(*this);
		listRight.registerMe(*this);
		listLeft.registerMe(*this);

		listRight.MoveMidpoint(1.0f);
		listLeft.MoveMidpoint(1.0f);

		listLeft.setMetrics({ 450.0f, 500.0f });
		listLeft.centerX(150.0f);

		listRight.setMetrics({ 350.0f, 500.0f });
		listRight.setOrigin(listLeft.getRect().TopRight() + PointF(10.0f, 0.0f));

		SendUsername();

		gs.pChat = std::unique_ptr< UIChatBox >(new UIChatBox(FONT_SMALL));
		gs.pChat->setOrigin({ 10.0f, 130.0f });
		gs.pChat->setMetrics({ 300.0f, 500.0f });

		AddObject(gs.pChat.get());
	}
	virtual ~ServerStartPage()
	{
		
	}

	virtual void ComposeFrame(Drawing& draw) override
	{
		muPage.Lock();

		lblTitle.draw(draw);

		if (listLeft.CountItems() > 0)
			listLeft.draw(draw);

		if (listRight.CountItems() > 0)
			listRight.draw(draw);



		muPage.Unlock();
		
		gs.pChat->draw(draw);
		btnDisconnect.draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		switch (gs.pClient->GetState())
		{
		case Client::States::Connected:
			for (const auto& c : gs.pClient->GetData())
			{
				try
				{
					ContainerReader r(c);
					HandleData(r);
				}
				catch (const std::logic_error& e)
				{
					Log::Warning(std::string("recieved invalid package: ") + e.what());
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

		for (auto& b : leftBtns)
		{
			if (b->Pushed())
			{
				SendButtonPress(b->GetID());
			}
		}

		if (gs.pChat->HasMessage())
		{
			std::string ms = gs.pChat->getMessage();
			StateGameMulti::SendChat(ms, *gs.pClient);
		}

		if (btnDisconnect.Pushed())
		{
			SetNextState(states::Menu);
		}
	}
	void HandleData(ContainerReader& r)
	{
		PacketType t = (PacketType)r.readShort();

		switch (t)
		{
		case PacketType::PageNewSection:
			HandleNewSection(r);
			break;
		case PacketType::PageUpdateLeft:
			muPage.Lock();
			HandleUpdateList(listLeft,leftLabels,leftBtns, r);
			muPage.Unlock();
			break;
		case PacketType::PageUpdateRight:
			muPage.Lock();
			HandleUpdateList(listRight,rightLabels,rightBtns, r);
			muPage.Unlock();
			break;
		case PacketType::ServerType:
			// redirecting
			HandleServerType(r);
			break;
		case PacketType::GameChat:
			HandleGameChat(r);
			break;
		}
	}
private:
	void SendUsername()
	{
		std::string name = Settings::GetName();
		DataContainer con = gs.pClient->GetConRel();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::Username);
		w.writeString(name.c_str());

		gs.pClient->SendReliable(std::move(con));
	}
	void HandleNewSection(ContainerReader& r)
	{
		curPage = r.readInt();
		LockGuard g(muPage);

		lblTitle.SetText(r.readString());
		lblTitle.AdjustToFont();
		lblTitle.centerX(50.0f);
	}
	void HandleUpdateList(UIItemListerScroll& list, std::vector< std::unique_ptr< UITextBox >>& lbls,
		std::vector< std::unique_ptr< UIButtonTextServer >>& btns, ContainerReader& r)
	{
		list.Clear();
		lbls.clear();
		btns.clear();

		int num = r.readInt();
		for (int i = 0; i < num; i++)
		{
			PageObject o = (PageObject)r.readByte();
			int id = r.readInt();
			std::string txt = r.readString();

			float wi = list.getMetrics().x - 30.0f;
			if (o == PageObject::Button)
			{
				// add button
				auto btn = std::unique_ptr< UIButtonTextServer >(new UIButtonTextServer(txt, id, FONT_MED));
				btn->setWidth(wi);
				list.AddItem(btn.get(), nullptr);
				btns.push_back(std::move(btn));
			}
			else if (o == PageObject::Label)
			{
				auto lbl = std::unique_ptr< UITextBox >(new UITextBox(txt, FONT_SMALL));
				lbl->SetColor(Color(id));
				lbl->AdjustToFont(wi);
				list.AddItem(lbl.get(), nullptr);
				lbls.push_back(std::move(lbl));
			}
			else if (o == PageObject::LabelBig)
			{
				auto lbl = std::unique_ptr< UITextBox >(new UITextBox(txt, FONT_MED));
				lbl->SetColor(Color(id));
				lbl->AdjustToFont(wi);
				list.AddItem(lbl.get(), nullptr);
				lbls.push_back(std::move(lbl));
			}
		}
		list.OrderItems();
	}
	void SendButtonPress(int id)
	{
		DataContainer con = gs.pClient->GetConRelSmall();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::ClientButtonpress);
		w.writeInt(id);

		gs.pClient->SendReliable(std::move(con));
	}
	void HandleServerType(ContainerReader& r)
	{
		int version = r.readInt();
		if (!Version::IsSupported(version))
			throw GameError("Server Version not supported: " + std::to_string(Version::GetGameVersion()));

		ServerTypes ty = (ServerTypes)r.readChar();
		if (ty != ServerTypes::Redirecting)
			return;
		
		short port = r.readShort();

		// redirect
		if (port == 0)
			throw GameError("no free slots available");

		std::string ip;
		Database::GetServerAdress(&ip, nullptr);
		Database::SetServerAdress(ip, port);

		gs.pClient->SendDisconnect();
		SetNextState(states::Connect);
	}
	void HandleGameChat(ContainerReader& r)
	{
		StateGameMulti::HandleChat(r, *gs.pChat);
	}
private:
	UILabel lblTitle;
	UIButtonText btnDisconnect;
	GameStruct& gs;

	Mutex muPage;
	int curPage = -1;

	UIItemListerScroll listLeft;
	UIItemListerScroll listRight;

	std::vector< std::unique_ptr< UIButtonTextServer >> leftBtns;
	std::vector< std::unique_ptr< UITextBox >> rightLabels;
	std::vector< std::unique_ptr< UIButtonTextServer >> rightBtns;
	std::vector< std::unique_ptr< UITextBox >> leftLabels;
};