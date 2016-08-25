#pragma once
#include "UIInfoLister.h"
#include "UILabel.h"
#include "UILabelNumber.h"
#include "Database.h"

class UIStatsLister : public UIInfoLister
{
	struct Item
	{
		UILabelNumber* lblEntSpawn = nullptr;
		UILabelNumber* lblEntDie = nullptr;
		UILabelNumber* lblPlanCap = nullptr;
		UILabelNumber* lblPlanLost = nullptr;
	};
	struct Caption
	{
		UILabel* lblEntSpawn = nullptr;
		UILabel* lblEntDie = nullptr;
		UILabel* lblPlanCap = nullptr;
		UILabel* lblPlanLost = nullptr;
	};
public:
	UIStatsLister()
	{}
	~UIStatsLister()
	{
		tool::safeDelete(cap.lblEntSpawn);
		tool::safeDelete(cap.lblEntDie);
		tool::safeDelete(cap.lblPlanCap);
		tool::safeDelete(cap.lblPlanLost);

		for (auto& e : itms)
		{
			tool::safeDelete(e.lblEntSpawn);
			tool::safeDelete(e.lblEntDie);
			tool::safeDelete(e.lblPlanCap);
			tool::safeDelete(e.lblPlanLost);
		}
	}
	void LoadGameResults(const GameResults& res)
	{
		MakeCaption();

		byte team = 1;
		for (const auto& e : res.data)
		{
			AddItem(e.entSpawn, e.entDie, e.planCap, e.planLost,team);
			team++;
		}
	}
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
protected:
	void AddItem(unsigned int eSpawn, unsigned int eLost, unsigned int pCap, unsigned int pLost, byte team)
	{
		Item i;
		i.lblEntSpawn = new UILabelNumber(FONT_SMALL, 10);
		i.lblEntSpawn->SetNumber(eSpawn);
		i.lblEntSpawn->SetColor(Color::GetTeamColor(team));
		i.lblEntSpawn->AdjustToFont();

		i.lblEntDie = new UILabelNumber(FONT_SMALL, 10);
		i.lblEntDie->SetNumber(eLost);
		i.lblEntDie->SetColor(Color::GetTeamColor(team));
		i.lblEntDie->AdjustToFont();

		i.lblPlanCap = new UILabelNumber(FONT_SMALL, 10);
		i.lblPlanCap->SetNumber(pCap);
		i.lblPlanCap->SetColor(Color::GetTeamColor(team));
		i.lblPlanCap->AdjustToFont();

		i.lblPlanLost = new UILabelNumber(FONT_SMALL, 10);
		i.lblPlanLost->SetNumber(pLost);
		i.lblPlanLost->SetColor(Color::GetTeamColor(team));
		i.lblPlanLost->AdjustToFont();

		float itmwi = (m_dim.x - 2 * padding) / 4.0f;
		const float pad = 10.0f;
		const float hei = cap.lblEntSpawn->getMetrics().y + 2 * pad;

		std::vector< UIInfoLister::Item > itm;
		UIInfoLister::Item n;
		n.pObj = i.lblEntSpawn;
		n.pos = PointF(2 * padding, pad);
		itm.push_back(n);

		n.pObj = i.lblEntDie;
		n.pos = PointF(2 * padding + itmwi, pad);
		itm.push_back(n);

		n.pObj = i.lblPlanCap;
		n.pos = PointF(2 * padding + 2 * itmwi, pad);
		itm.push_back(n);

		n.pObj = i.lblPlanLost;
		n.pos = PointF(2 * padding + 3 * itmwi, pad);
		itm.push_back(n);

		UIInfoLister::AddItem(std::move(itm), hei);
		itms.push_back(i);
	}
	void MakeCaption()
	{
		float itmwi = (m_dim.x - 2 * padding) / 4.0f;

		cap.lblEntSpawn = new UILabel("Spawned Units:", FONT_SMALL);
		cap.lblEntDie = new UILabel("Lost Units:", FONT_SMALL);
		cap.lblPlanCap = new UILabel("Captured:", FONT_SMALL);
		cap.lblPlanLost = new UILabel("Lost Planets:", FONT_SMALL);

		const float pad = 10.0f;
		const float hei = cap.lblEntSpawn->getMetrics().y + 2 * pad;

		std::vector< UIInfoLister::Item > itm;
		UIInfoLister::Item n;
		n.pObj = cap.lblEntSpawn;
		n.pos = PointF(2 * padding, pad);
		itm.push_back(n);

		n.pObj = cap.lblEntDie;
		n.pos = PointF(2 * padding + itmwi, pad);
		itm.push_back(n);

		n.pObj = cap.lblPlanCap;
		n.pos = PointF(2 * padding + 2 * itmwi, pad);
		itm.push_back(n);

		n.pObj = cap.lblPlanLost;
		n.pos = PointF(2 * padding + 3 * itmwi, pad);
		itm.push_back(n);

		UIInfoLister::AddItem(std::move(itm), hei);
	}

private:
	std::vector<Item> itms;
	bool bHover = false;
	bool bDown = false;
	PointF lastPos;
	Caption cap;
};