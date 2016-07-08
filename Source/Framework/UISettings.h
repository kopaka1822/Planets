#pragma once
#include "UIItemListerScroll.h"
#include "UIButtonTextBorderless.h"

class UISettings : public UIObject
{
	struct Item
	{
		UIButtonTextBorderless* pBtn = nullptr;
		UIItemListerScroll* pScroll = nullptr;
	};
public:
	// does not delete scrolls
	UISettings()
	{

	}
	virtual ~UISettings()
	{
		for (auto& i : itms)
		{
			tool::safeDelete(i.pBtn);
		}
	}
	void AddSection(const std::string& str, UIItemListerScroll* pScroll)
	{
		Item i;
		i.pBtn = new UIButtonTextBorderless(str, FONT_MED);
		i.pScroll = pScroll;

		itms.push_back(i);
	}
	virtual void Register(GameState& gs) override
	{
		for (auto& i : itms)
		{
			i.pBtn->Register(gs);
			i.pScroll->Register(gs);
		}
	}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawBox(GetRect(), (float)border, Color::White(), Color::Black());

		assert(itms.size());
		float dy = border + itms[0].pBtn->GetMetrics().y;
		PointF start(pos.x + border, pos.y + dy + border / 2);
		for (auto& i : itms)
		{
			i.pBtn->Draw(draw);
			if (i.pScroll->isEnabled())
			{
				i.pScroll->Draw(draw);
			}

			draw.DrawHLine(start, btnWi, (float)border, Color::White());
			start.y += dy;
		}

		draw.DrawVLine(pos + PointF(1.5f * border + btnWi, border), dim.y - 2 * border, (float)border,Color::White());
	}
	// call this after origin!
	virtual void SetMetrics(const PointF& m) override
	{
		UIObject::SetMetrics(m);

		btnWi = 0.0f;
		for (const auto& i : itms)
		{
			btnWi = std::max(btnWi, i.pBtn->GetMetrics().x);
		}

		float curY = pos.y + border;
		const float xbtn = pos.x + border;
		float xscroll = pos.x + btnWi + 2 * border + padding;
		float yscroll = pos.y + border + padding;

		float scrollwi = dim.x - 3 * border - btnWi - 2 * padding;
		float scrollhi = dim.y - 2 * border - 2 * padding;
		for (auto& i : itms)
		{
			i.pBtn->SetWidth(btnWi);
			i.pBtn->SetOrigin(PointF(xbtn, curY));
			curY += i.pBtn->GetMetrics().y + border;

			i.pScroll->SetMetrics(PointF(scrollwi, scrollhi));
			i.pScroll->SetOrigin(PointF(xscroll, yscroll));
		}
	}
	void OrderItems()
	{
		for (auto& i : itms)
		{
			i.pScroll->OrderItems();
			//i.pScroll->MoveCam(1.0f);
			i.pScroll->Disable();
		}

		if (itms.size())
		{
			itms[0].pBtn->SetActive(true);
			itms[0].pScroll->Enable();
		}
	}
	void Update()
	{
		for (auto& i : itms)
		{
			if (i.pBtn->Pushed() && !i.pBtn->isActive())
			{
				//change active
				for (auto& it : itms)
				{
					if (it.pBtn->isActive())
					{
						it.pScroll->Disable();
						it.pBtn->SetActive(false);
					}	
				}
				i.pBtn->SetActive(true);
				i.pScroll->Enable();
			}
		}
	}
private:
	float btnWi = 0.0f;
	std::vector<Item> itms;
	static const int border = 6;
	static const int padding = 8;
};