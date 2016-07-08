#pragma once
#include "UIInfoLister.h"
#include "UILabel.h"
#include "UIButtonText.h"
#include "UIButtonCross.h"

class UIServerLister : public UIInfoLister
{
	struct Item
	{
		UILabel* lblServ = nullptr;
		UILabel* lblPort = nullptr;
		UIButtonText* btnConnect = nullptr;
		UIButtonCross* btnCross = nullptr;
		std::string ip;
		unsigned short port;
	};
public:
	UIServerLister()
	{}

	// Doesnt delete existing items!
	void LoadFromSettings()
	{
		const auto& s = Settings::GetServerInfos();
		for (const auto& i : s)
		{
			AddItem(i.IP, std::to_string((int)i.port), i.nick);
		}
	}
	virtual ~UIServerLister()
	{
		std::vector<Settings::ServerInfo> sinf;
		for (auto& i : itms)
		{
			if (i.lblServ != nullptr)
			{
				Settings::ServerInfo s;
				s.nick = i.lblServ->GetText();
				s.IP = i.ip;
				s.port = i.port;
				sinf.push_back(std::move(s));
				tool::safeDelete(i.lblServ);
			}
			tool::safeDelete(i.lblPort);
			tool::safeDelete(i.btnConnect);
			tool::safeDelete(i.btnCross);
		}
		Settings::SetServerInfos(std::move(sinf));
	}
	void AddItem(const std::string& serv, const std::string& port, const std::string& nickname)
	{
		const float hei = 100.0f;
		const float wi = dim.x - 2 * padding;
		Item i;
		i.lblServ = new UILabel(nickname, FONT_SMALL);
		i.lblPort = new UILabel(serv + std::string(":") + port, FONT_SMALL);
		i.btnConnect = new UIButtonText("connect", FONT_SMALL);
		i.btnCross = new UIButtonCross();
		i.ip = serv;
		i.port = (unsigned short)atoi(port.c_str());

		i.btnConnect->AdjustToFont();

		std::vector< UIInfoLister::Item > itm;
		UIInfoLister::Item n;
		n.pObj = i.lblServ;
		n.pos = PointF(2 * padding, padding);
		itm.push_back(n);

		n.pObj = i.lblPort;
		n.pos = PointF( 2 * padding, hei / 2.0f);
		itm.push_back(n);

		n.pObj = i.btnConnect;
		n.pos = PointF(wi - 2 *  padding,hei - padding) - i.btnConnect->GetMetrics();
		itm.push_back(n);

		const float sz = 30.0f;
		i.btnCross->SetMetrics({ sz, sz });
		n.pObj = i.btnCross;
		n.pos = { wi - 2 * padding - sz, padding };
		itm.push_back(n);

		UIInfoLister::AddItem(std::move(itm), hei);
		itms.push_back(i);
	}
	virtual void Event_KeyDown(SDL_Scancode code) override
	{
		for (auto& i : itms)
		{
			if (i.btnConnect->isEnabled())
				i.btnConnect->Event_KeyDown(code);
		}
	}
	virtual void Event_KeyUp(SDL_Scancode code) override
	{
		for (auto& i : itms)
		{
			if (i.btnConnect->isEnabled())
				i.btnConnect->Event_KeyUp(code);
		}
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		for (auto& i : itms)
		{
			if (i.btnConnect->isEnabled())
				i.btnConnect->Event_MouseDown(k, pos);

			if (i.btnCross->isEnabled())
				i.btnCross->Event_MouseDown(k, pos);
		}

		if (k == Input::Left && bHover)
		{
			bDown = true;
			lastPos = pos;
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		for (auto& i : itms)
		{
			if (i.btnConnect->isEnabled())
				i.btnConnect->Event_MouseUp(k, pos);

			if (i.btnCross->isEnabled())
				i.btnCross->Event_MouseUp(k, pos);
		}

		bDown = false;
	}
	virtual void Event_MouseMove(const PointF& pos) override
	{
		for (auto& i : itms)
		{
			if (i.btnConnect->isEnabled())
				i.btnConnect->Event_MouseMove(pos);

			if (i.btnCross->isEnabled())
				i.btnCross->Event_MouseMove(pos);
		}

		bHover = GetRect().PointInside(pos);
		if (bDown)
		{
			float dy = pos.y - lastPos.y;
			lastPos = pos;
			UIInfoLister::MoveCam(-dy);
		}
	}
	void Update()
	{
		unsigned int index = 0;
		for (auto i = itms.begin(), end = itms.end(); i != end; i++)
		{
			if (i->btnCross->Pushed())
			{
				UIInfoLister::DeleteItem(index);
				tool::safeDelete(i->btnConnect);
				tool::safeDelete(i->btnCross);
				tool::safeDelete(i->lblPort);
				tool::safeDelete(i->lblServ);
				itms.erase(i);

				Event_MouseMove(Input::GetMouseXY());
				break;
			}
			index++;
		}
	}
	bool ServerChoosen(std::string* dstIp, unsigned short* dstPort)
	{
		for (const auto& i : itms)
		{
			if (i.btnConnect->Pushed())
			{
				*dstIp = i.ip;
				*dstPort = i.port;
				return true;
			}
		}
		return false;
	}
	virtual void Event_MouseWheel(float amount, const PointF& pos) override
	{
		if (bHover)
			UIInfoLister::MoveCam(-(float)amount * 20.0f);
	}
private:
	std::vector< Item > itms;
	bool bHover = false;
	bool bDown = false;
	PointF lastPos;
};
