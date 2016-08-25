#pragma once
#include "UIObject.h"
#include "Database.h"
#include "UIClanButton.h"

class UIIngamePlayerList : public UIObject
{
public:
	UIIngamePlayerList(int nPlayer, Font& font, bool allConst = false)
		:
		font(font),
		nPlayer(nPlayer),
		names(Database::GetGameStruct().playernames),
		namelen(int(font.GetFontWidth() + 0.5f) * (MAX_PLAYERLEN + 4)), // 4 = ALLY
		myTeam(Database::GetGameStruct().myTeam)
	{
		btns.assign(nPlayer, nullptr);
		for (auto& b : btns)
		{
			b = new UIClanButton(font);
			if (allConst)
				b->SetConst(true);
		}
		btns[myTeam - 1]->SetState(UIClanButton::state::ally);
		btns[myTeam - 1]->SetConst(true);
		itmhei = (int)btns[0]->getMetrics().y;
	}
	virtual ~UIIngamePlayerList()
	{
		for (auto& b : btns)
		{
			tool::safeDelete(b);
		}
	}
	virtual void draw(Drawing& draw) override
	{
		if (!bVisible)
			return;

		draw.DrawBox(getRect(), (float)border, Color::GetTeamColor(myTeam), Color::Black());
		draw.DrawVLine(m_pos + PointF(m_dim.x / 2.0f - name_padd / 2.0f + padding, 2.0f * border), m_dim.y - 4.0f * border, (float)border, Color::Gray());

		// draw names
		float leftx = m_pos.x + border + padding;
		float rightx = leftx + namelen + padding + name_padd;

		float curY = m_pos.y + border + 1 * padding;

		font.SetColor(Color::White());
		font.Text("Ally", PointF(leftx + MAX_PLAYERLEN * font.GetFontWidth(), curY));
		font.Text("Ally", PointF(rightx + MAX_PLAYERLEN * font.GetFontWidth(), curY));
		curY += font.GetFontHeight() + padding;

		for (int i = 0; i < nPlayer; ++i)
		{
			btns[i]->draw(draw);
			font.SetColor(Color::GetTeamColor(i + 1));
			if (i % 2 == 0)
			{
				// left
				font.Text(names[i], PointF(leftx, curY + 4));
			}
			else
			{
				// right
				font.Text(names[i], PointF(rightx, curY + 4));
				curY += itmhei + padding;
			}
		}
	}
	// metric
	void AdjustToPlayers()
	{
		m_dim.x = 2.0f * border + 3 * padding + 2 * namelen + 2 * name_padd;
		int playerrows = (nPlayer + 1) / 2;
		m_dim.y = 2.0f * border + (playerrows + 1) * itmhei + (playerrows + 2) * padding;
	}
	// position
	void AdjustToScreen()
	{
		center();
		// adjust buttons
		float leftx = m_pos.x + border + padding + font.GetFontWidth() * MAX_PLAYERLEN;
		float rightx = m_pos.x + border + padding + namelen + name_padd + padding + font.GetFontWidth() * MAX_PLAYERLEN;

		float curY = m_pos.y + border + 1 * padding + font.GetFontHeight() + padding;

		for (int i = 0; i < nPlayer; ++i)
		{
			if (i % 2 == 0)
			{
				// left
				btns[i]->setOrigin({ leftx, curY });
			}
			else
			{
				// right
				btns[i]->setOrigin({ rightx, curY });
				curY += itmhei + padding;
			}
		}
	}
	virtual void Event_GameKeyDown(Input::GameKey k, const PointF& p) override
	{
		if (k == Input::GameKey::GK_PLAYERLIST)
		{
			SetVisible(true);
		}
	}
	virtual void Event_GameKeyUp(Input::GameKey k, const PointF& p) override
	{
		if (k == Input::GameKey::GK_PLAYERLIST)
		{
			SetVisible(false);
		}
	}

	std::vector< UIClanButton* >& GetButtons()
	{
		return btns;
	}

	bool isVisible() const
	{
		return bVisible;
	}
	void SetVisible(bool vis)
	{
		if (vis)
		{
			bVisible = true;
			EnableAll();
		}
		else
		{
			bVisible = false;
			DisableAll();
		}
	}
private:
	void EnableAll()
	{
		for (auto& b : btns)
		{
			b->enable();
		}
	}
	void DisableAll()
	{
		for (auto& b : btns)
		{
			b->disable();
		}
	}
private:
	const byte myTeam;
	const int nPlayer;
	const int namelen;
	int itmhei;
	const std::vector< std::string >& names;
	static const int MAX_PLAYERLEN = 20;
	static const int border = 10;
	static const int padding = 8;
	static const int name_padd = 30;
	Font& font;

	std::vector< UIClanButton* > btns;

	bool bVisible = false;
};