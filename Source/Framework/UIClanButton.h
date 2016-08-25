#pragma once
#include "UIButton.h"

class UIClanButton : public UIButton
{
public:
	enum class state
	{
		ally,
		noAlly,
		awaitingResponse,
		sendResponse
	};
public:
	UIClanButton(Font& font)
		:
		font(font),
		curState(state::noAlly)
	{
		AdjustToFont();
	}
	void SetState(state s)
	{
		curState = s;
	}
	state GetState() const
	{
		return curState;
	}
	void AdjustToFont()
	{
		m_dim.x = font.GetFontHeight() + 2 * border + 2 * padding;
		m_dim.y = font.GetFontHeight() + 2 * border + 2 * padding;
	}
	virtual void draw(Drawing& draw) override
	{
		if (curState != state::ally)
			draw.DrawBox(getRect(), (float)border, Color::White(), Color::Black());

		font.SetColor(Color::White());
		PointF tpos = m_pos + PointF(border + padding + (font.GetFontHeight() - font.GetFontWidth()) / 2
			, border + padding);

		switch (curState)
		{
		case state::ally:
			draw.DrawRect(getRect(), Color::White());
			break;
		case state::awaitingResponse:
			font.Text("A", tpos);
			break;
		case state::sendResponse:
			font.Text("R", tpos);
			break;
		}
		
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		if (!isConst)
		{
			UIButton::Event_MouseDown(k, pos);
		}
	}
	void SetConst(bool c)
	{
		isConst = c;
	}
	bool Changed()
	{
		bool r = bChange;
		bChange = false;
		return r;
	}
private:
	Font& font;
	static const int border = 2;
	static const int padding = 2;
	state curState;
	bool isConst = false;
	bool bChange = false;
};