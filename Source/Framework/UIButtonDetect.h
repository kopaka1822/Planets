#pragma once
#include "UIButton.h"

class UIButtonDetect : public UIButton
{
public:
	UIButtonDetect(Font& f, Input::GKstruct ks)
		:
		font(f)
	{
		SetKey(ks);
	}
	virtual void AdjustToFont(float width)
	{
		SetMetrics({ width, 2 * (padding + border) + font.GetFontHeight() });
	}
	static std::string KeyToString(Input::GKstruct k)
	{
		std::string txt;
		if (k.t == Input::GKstruct::Key)
		{
			txt = Input::KeyToAscii(k.code);
		}
		else
		{
			switch (k.code)
			{
			case Input::Left:
				txt = "left mouse";
				break;
			case Input::Right:
				txt = "right mouse";
				break;
			case Input::Middle:
				txt = "middle mouse";
				break;
			case Input::ScrollDown:
				txt = "scroll down";
				break;
			case Input::ScrollUp:
				txt = "scroll up";
				break;
			case Input::X1:
				txt = "mouse x1";
				break;
			case Input::X2:
				txt = "mouse x2";
				break;
			default:
				txt = "?";
			}
		}
		return txt;
	}
	void SetKey(Input::GKstruct k)
	{
		muTxt.Lock();
		key = k;
		txt = KeyToString(k);
		muTxt.Unlock();
	}
	Input::GKstruct GetKey() const
	{
		return key;
	}
	virtual void Event_KeyDown(SDL_Scancode code) override
	{
		if (bHover)
		{
			key.code = code;
			key.t = Input::GKstruct::Key;
			SetKey(key); // generate string
		}
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		UIButton::Event_MouseDown(k, pos);

		if (bHover)
		{
			key.code = k;
			key.t = Input::GKstruct::Mouse;
			SetKey(key); // generate string
		}
	}
	virtual void Draw(Drawing& draw) override
	{
		UIButton::Draw(draw);
		muTxt.Lock();
		std::string s = txt;
		muTxt.Unlock();

		font.Text(s, CalculateFontPos(s));
	}
private:
	PointF CalculateFontPos(const std::string& txt) const
	{
		PointF p;
		p.x = dim.x - 2 * border - 2 * padding - font.GetFontWidth() * txt.length();
		p.x *= 0.5f;
		p.x += border + padding;
		p.y = dim.y - 2 * border - 2 * padding - font.GetFontHeight();
		p.y *= 0.5f;
		p.y += border + padding;

		return pos + p;
	}
private:
	const int padding = 4;
	Font& font;

	Input::GKstruct key;
	std::string txt;

	Mutex muTxt;
};