#pragma once
#include "UIInfoLister.h"
#include "UILabel.h"
#include "UILabelNumber.h"
#include "UIParticle.h"
#include "../Utility/Tools.h"

class UIPlayerLister : public UIInfoLister
{
	struct Item
	{
		UILabel* lblPlayer = nullptr;
		UILabelNumber* lblPing = nullptr;
		UIParticle* part = nullptr;
		int team;
	};
public:
	UIPlayerLister()
	{}
	virtual ~UIPlayerLister()
	{
		for (auto& i : elems)
		{
			tool::safeDelete(i.lblPlayer);
			tool::safeDelete(i.lblPing);
			tool::safeDelete(i.part);
		}
	}
	void Clear()
	{
		UIInfoLister::Clear();
		for (auto& i : elems)
		{
			tool::safeDelete(i.lblPlayer);
			tool::safeDelete(i.lblPing);
			tool::safeDelete(i.part);
		}
	}
	void AddItem(const std::string& player, float ping, int team, bool ready)
	{
		const float hei = 60.0f;
		const float wi = m_dim.x - 2 * padding;
		Item i;
		i.lblPlayer = new UILabel(player, FONT_SMALL);
		i.lblPlayer->SetColor(Color::GetTeamColor(team));
		i.lblPing = new UILabelNumber(FONT_SMALL, 5);
		i.lblPing->SetColor(Color::GetTeamColor(team));
		i.lblPing->SetNumber((int)ping);
		i.part = new UIParticle(ready ? Color::Green() : Color::Red());
		i.part->setMetrics({ hei, hei });

		std::vector< UIInfoLister::Item > itm;
		UIInfoLister::Item n;
		n.pObj = i.lblPlayer;
		n.pos = PointF(2 * padding, padding);
		itm.push_back(n);

		n.pObj = i.lblPing;
		n.pos = PointF(wi - 2 * padding, hei - padding) - i.lblPing->getMetrics();
		itm.push_back(n);

		n.pObj = i.part;
		n.pos = PointF(wi - 3 * padding - i.lblPing->getMetrics().x - i.part->getMetrics().x,
			0);
		itm.push_back(n);

		UIInfoLister::AddItem(std::move(itm),hei);
		elems.push_back(i);
	}
	// mouse etc.
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		if (k == Input::Left && bHover)
		{
			bDown = true;
			lastPos = pos;
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		bDown = false;
	}
	virtual void Event_MouseMove(const PointF& pos) override
	{
		bHover = getRect().PointInside(pos);
		if (bDown)
		{
			float dy = pos.y - lastPos.y;
			lastPos = pos;
			UIInfoLister::MoveCam(-dy);
		}
	}
	virtual void Event_MouseWheel(float amount, const PointF& pos) override
	{
		if (bHover)
			UIInfoLister::MoveCam(-(float)amount * 20.0f);
	}
private:
	std::vector< Item > elems;
	bool bDown = false;
	bool bHover = false;
	PointF lastPos;
};