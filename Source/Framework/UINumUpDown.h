#pragma once
#include "UIObject.h"
#include "UITextInput.h"

template <class T>
class UINumUpDown : public UIObject
{
public:
	UINumUpDown(T start, T min, T max, T step, Font& font)
		:
		value(start),
		mi(min), ma(max),
		font(font), step(step),
		input(font,20)
	{
		input.Disable();
	}
	virtual void Disable() override
	{
		input.Disable();
		UIObject::Disable();
	}
	virtual void SetOrigin(const PointF& o) override
	{
		UIObject::SetOrigin(o);
		input.SetOrigin(pos + PointF(border + padding, border + padding));
	}
	virtual void SetMetrics(const PointF& p) override
	{
		UIObject::SetMetrics(p);
		input.SetMetrics(dim - PointF(2 * (border + padding), 2 * (border + padding)));
	}
	virtual void Register(GameState& gs) override
	{
		input.Register(gs);
		UIObject::Register(gs);
	}
	void AdjustToFont(float width)
	{
		dim.x = width;
		dim.y = float(border + padding) * 2 + font.GetFontHeight();
		SetMetrics(dim);
	}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawBox(GetRect(), (float)border, Color::White(), Color::Black());

		PointF tpos = pos + PointF(padding + border, padding + border);

		font.SetColor(Color::White());

		if (!input.isEnabled())
			DrawNum(font, tpos);

		// Draw the buttons
		if (isEnabled() && !input.isEnabled())
		{
			//border
			draw.DrawRect(RectF(pos + dim + PointF(-2 * border - font.GetFontHeight(), -dim.y),
				pos + dim + PointF(-border - font.GetFontHeight(), 0)), Color::White());

			//line through mid
			if (bHover1) draw.DrawRect(RectTop(), Color::Gray());
			if (bHover2) draw.DrawRect(RectBot(), Color::Gray());

			DrawArrow(draw);
		}
		if (input.isEnabled())
			input.Draw(draw);

	}
	virtual void Event_MouseMove(const PointF& pos) override
	{
		bHover1 = RectTop().PointInside(pos);
		bHover2 = RectBot().PointInside(pos) && !bHover1;

		bHover = GetRect().PointInside(pos);
		input.Event_MouseMove(pos);
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& p) override
	{
		input.Event_MouseDown(k, p);
		if (bEnable && !input.isEnabled())
		{
			if (bHover1)
			{
				SetValue(value + step);
				Sound::Effect(Sound::S_CLICK);
			}
			else if (bHover2)
			{
				SetValue(value - step);
				Sound::Effect(Sound::S_CLICK);
			}
			else if (bHover)
			{
				// activate input

				// copy text to input
				input.SetText(NumToString());
				input.Enable();
				input.Select();
			}
		}
		TestInput();
	}
	virtual void Event_KeyDown(SDL_Scancode code) override
	{
		input.Event_KeyDown(code);
		TestInput();
	}
	virtual void Event_KeyUp(SDL_Scancode code) override
	{
		input.Event_KeyUp(code);
		TestInput();
	}
	virtual void Event_Char(char c) override
	{
		input.Event_Char(c);
	}
	void SetValue(T val)
	{
		if(SetValueNoChange(val))
			bChange = true;
	}
	virtual bool SetValueNoChange(T val)
	{
		T prev = value;
		value = val;
		value = std::min(ma, value);
		value = std::max(mi, value);
		bChange = false;

		return value != prev;
	}
	bool ValueChanged()
	{
		bool r = bChange;
		bChange = false;
		return r;
	}
	T GetValue() const
	{
		return value;
	}
private:
	void DrawArrow(Drawing& draw)
	{
		//1st
		RectF r = RectTop();
		PointF l, m, ri;
		l = r.BottomLeft() + PointF(4,-4);
		ri = r.BottomRight() + PointF(-4,- 4);
		m = (r.TopLeft() + r.TopRight()) / 2.0f + PointF(0, 4);

		draw.DrawLine(l, m, Color::White(), 2.0f);
		draw.DrawLine(m, ri, Color::White(), 2.0f);

		r = RectBot();
		l = r.TopLeft() + PointF(4, 4);
		ri = r.TopRight() + PointF(-4, 4);
		m = (r.BottomLeft() + r.BottomRight()) / 2.0f + PointF(0,-4);

		draw.DrawLine(l, m, Color::White(), 2.0f);
		draw.DrawLine(m, ri, Color::White(), 2.0f);
	}
	void TestInput()
	{
		if (input.isEnabled())
		{
			if (!input.isSelected())
			{
				// finished typing
				if (input.GetText().length() > 0)
				{
					SetValue(StringToNum(input.GetText()));
				}
				input.Disable();
			}
		}
	}
protected:
	virtual void DrawNum(Font& f, const PointF& start) const = 0;
	virtual std::string NumToString() const = 0;
	virtual T StringToNum(const std::string& s) = 0;

	RectF RectTop() const
	{
		return RectF(pos + dim + PointF(-border - font.GetFontHeight(), -dim.y + border),
			pos + dim + PointF(- border, -dim.y / 2));
	}
	RectF RectBot() const
	{
		return RectTop() + PointF(0, dim.y / 2 - border);
	}
	int GetMaxCharWidth() const
	{
		int len = (int)dim.x - 2 * border;
		if (bEnable)
			len = len - border - (int)font.GetFontHeight();
		return len / (int)font.GetFontWidth();
	}
	std::string CutString(const std::string& t) const
	{
		std::string ret = t.substr(0, std::min(t.length(), (size_t)GetMaxCharWidth() - 1));
		if (ret.length() < t.length() && ret.length() > 2)
		{
			//search for point
			if (ret.find('.') == std::string::npos)
			{
				ret.pop_back();
				ret.pop_back();
				ret += "..";
			}

		}
		return ret;
	}

protected:
	T value, step;
	T mi, ma; //min & max
	Font& font;

	static const int border = 6;
	static const int padding = 4;

	bool bHover = false;
	bool bHover1 = false;
	bool bHover2 = false;
	bool bChange = false;
	bool bEnable = true;

	UITextInput input;
};
