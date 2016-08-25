#pragma once
#include "UIInfoLister.h"
#include "UILabel.h"
#include "../Utility/Tools.h"

class UIAchievLister : public UIInfoLister
{
	struct Item
	{
		UILabel* lblTitle = nullptr;
		UILabel* lblDesc = nullptr;
	};

public:
	UIAchievLister()
	{}
	void LoadData()
	{
		for (const auto& e : Settings::GetAchievements())
		{
			if (e.unlocked)
				AddItem(e.title, e.desc);
		}
	}
	virtual ~UIAchievLister()
	{
		for (auto& i : itms)
		{
			tool::safeDelete(i.lblTitle);
			tool::safeDelete(i.lblDesc);
		}
	}
private:
	void AddItem(const std::string& tit, const std::string desc)
	{
		const float hei = 3 * padd + FONT_MED.GetFontHeight() + FONT_SMALL.GetFontHeight();
		const float padSide = 20;
		Item i;
		i.lblTitle = new UILabel(tit, FONT_MED);
		i.lblDesc = new UILabel(desc, FONT_SMALL);

		std::vector< UIInfoLister::Item > itm;
		UIInfoLister::Item n;
		n.pObj = i.lblTitle;
		n.pos = PointF(padSide, padding);
		itm.push_back(n);

		n.pObj = i.lblDesc;
		n.pos = PointF(padSide, 2 * padding + i.lblTitle->getMetrics().y);
		itm.push_back(n);

		UIInfoLister::AddItem(std::move(itm), hei);
		itms.push_back(i);

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
	std::vector< Item > itms;
	static const int padd = 6;

	bool bDown = false;
	bool bHover = false;
	PointF lastPos;
};