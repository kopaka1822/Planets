#pragma once
#include "GameState.h"
#include "UIServerLister.h"
#include "UIBoxInput.h"

class StateServerSelect : public GameState
{
public:
	StateServerSelect()
		:
		lblTitle("Select Server", FONT_BIG),
		btnBack("Back", FONT_MED),

		lblServer("Server:", FONT_SMALL),
		lblPort("Port:", FONT_SMALL),
		lblNickname("Nickname:",FONT_SMALL),

		btnAddServer("Add Server", FONT_SMALL),
		btnDirect("Direct Connect", FONT_SMALL),

		inpServer(FONT_SMALL,100),
		inpPort(FONT_SMALL,5),
		inpNickname(FONT_SMALL,15)
	{
		lblTitle.centerX(50.0f);

		btnBack.AdjustToFont();
		btnBack.setOrigin(PointF(10.0f, Framework::STD_DRAW_Y - 10.0f - btnBack.getMetrics().y));

		AddObject(&btnBack);

		list.setMetrics(PointF(800, 550));
		list.setOrigin({ 350.0f, 150.0f });

		AddObject(&list);
		AddObject(&btnAddServer);
		AddObject(&btnDirect);
		AddObject(&inpServer);
		AddObject(&inpPort);
		AddObject(&inpNickname);

		//buttons
		float wi = 260.0f;
		inpServer.Adjust(wi);
		inpPort.Adjust(wi);
		inpNickname.Adjust(wi);

		btnAddServer.setWidth(wi);
		btnDirect.setWidth(wi);

		AlignObjects(PointF(50.0f, 150.0f), 10.0f, std::initializer_list < UIObject* > {
			&lblServer, &inpServer,
				&lblPort, &inpPort,
				&btnDirect,
				&lblNickname, &inpNickname,
				&btnAddServer
		});

		inpServer.SetFlags(UITextInput::F_SERVER);
		inpPort.SetFlags(UITextInput::F_NUMBER);

		inpPort.SetText("7123"); //Default port

		list.LoadFromSettings();

		Database::SetMainServer("", 0); // no main server
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.draw(draw);
		
		lblServer.draw(draw);
		lblPort.draw(draw);
		lblNickname.draw(draw);

		btnAddServer.draw(draw);
		btnDirect.draw(draw);

		inpServer.draw(draw);
		inpNickname.draw(draw);
		inpPort.draw(draw);
		
		btnBack.draw(draw);



		list.draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnBack.Pushed())
			SetNextState(states::Menu);

		list.Update();

		if (btnDirect.Pushed())
		{
			bool okay = true;
			if (inpServer.GetText().size() == 0)
			{
				okay = false;
				inpServer.MarkRed();
			}

			if (inpPort.GetText().size() == 0)
			{
				okay = false;
				inpPort.MarkRed();
			}

			if (okay)
			{
				Database::SetServerAdress(inpServer.GetText(), atoi(inpPort.GetText().c_str()));
				SetNextState(states::Connect);
				return;
			}
		}

		if (btnAddServer.Pushed())
		{
			bool okay = true;
			if (inpServer.GetText().size() == 0)
			{
				okay = false;
				inpServer.MarkRed();
			}

			if (inpPort.GetText().size() == 0)
			{
				okay = false;
				inpPort.MarkRed();
			}

			if (inpNickname.GetText().size() == 0)
			{
				okay = false;
				inpNickname.MarkRed();
			}

			if (okay)
			{
				list.AddItem(inpServer.GetText(), inpPort.GetText(), inpNickname.GetText());
			}
		}

		unsigned short p;
		std::string ip;

		if (list.ServerChoosen(&ip, &p))
		{
			Database::SetServerAdress(ip, p);
			SetNextState(states::Connect);
		}
	}
private:
	UILabel lblTitle;
	UIButtonText btnBack;
	UIServerLister list;

	UILabel lblServer;
	UILabel lblPort;
	UILabel lblNickname;
	
	UIButtonText btnAddServer;
	UIButtonText btnDirect;

	UIBoxInput inpServer;
	UIBoxInput inpPort;
	UIBoxInput inpNickname;
};