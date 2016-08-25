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
	virtual void registerMe(GameState& gs) override
	{
		for (auto& i : itms)
		{
			i.pBtn->registerMe(gs);
			i.pScroll->registerMe(gs);
		}
	}
	virtual void draw(Drawing& draw) override
	{
		draw.DrawBox(getRect(), (float)border, Color::White(), Color::Black());

		assert(itms.size());
		float dy = border + itms[0].pBtn->getMetrics().y;
		PointF start(m_pos.x + border, m_pos.y + dy + border / 2);
		for (auto& i : itms)
		{
			i.pBtn->draw(draw);
			if (i.pScroll->isEnabled())
			{
				i.pScroll->draw(draw);
			}

			draw.DrawHLine(start, btnWi, (float)border, Color::White());
			start.y += dy;
		}

		draw.DrawVLine(m_pos + PointF(1.5f * border + btnWi, border), m_dim.y - 2 * border, (float)border,Color::White());
	}
	// call this after origin!
	virtual void setMetrics(const PointF& m) override
	{
		UIObject::setMetrics(m);

		btnWi = 0.0f;
		for (const auto& i : itms)
		{
			btnWi = std::max(btnWi, i.pBtn->getMetrics().x);
		}

		float curY = m_pos.y + border;
		const float xbtn = m_pos.x + border;
		float xscroll = m_pos.x + btnWi + 2 * border + padding;
		float yscroll = m_pos.y + border + padding;

		float scrollwi = m_dim.x - 3 * border - btnWi - 2 * padding;
		float scrollhi = m_dim.y - 2 * border - 2 * padding;
		for (auto& i : itms)
		{
			i.pBtn->setWidth(btnWi);
			i.pBtn->setOrigin(PointF(xbtn, curY));
			curY += i.pBtn->getMetrics().y + border;

			i.pScroll->setMetrics(PointF(scrollwi, scrollhi));
			i.pScroll->setOrigin(PointF(xscroll, yscroll));
		}
	}
	void OrderItems()
	{
		for (auto& i : itms)
		{
			i.pScroll->OrderItems();
			//i.pScroll->MoveCam(1.0f);
			i.pScroll->disable();
		}

		if (itms.size())
		{
			itms[0].pBtn->SetActive(true);
			itms[0].pScroll->enable();
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
						it.pScroll->disable();
						it.pBtn->SetActive(false);
					}	
				}
				i.pBtn->SetActive(true);
				i.pScroll->enable();
			}
		}
	}
private:
	float btnWi = 0.0f;
	std::vector<Item> itms;
	static const int border = 6;
	static const int padding = 8;
};