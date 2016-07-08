#pragma once

#include "UITextInput.h"
#include "UIButton.h"

class UIBoxInput : public UIButton
{
public:
	UIBoxInput(Font& font, int maxLen = 30)
		:
		TextInput(font,maxLen)
	{}
	void Adjust(float width)
	{
		SetMetrics({ width, TextInput.GetFontHeight() + 2.0f * (float)border });
	}
	void SetFlags(UITextInput::FLAGS f)
	{
		TextInput.SetFlags(f);
	}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawBox(GetRect(), (float)border,mark ? Color::Red() : Color::White(), Color::Black());
		TextInput.Draw(draw);
	}
	virtual void SetOrigin(const PointF& p) override
	{
		TextInput.SetOrigin(p + PointF(border, border));
		pos = p;
	}
	virtual void SetMetrics(const PointF& d) override
	{
		TextInput.SetMetrics(d);
		dim = d;
	}
	void SetText(std::string NewText)
	{
		TextInput.SetText(NewText);
	}
	virtual void Event_Char(char c) override
	{
		TextInput.Event_Char(c);
	}
	virtual void Event_MouseMove(const PointF& mpos) override
	{
		TextInput.Event_MouseMove(mpos);
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		TextInput.Event_MouseDown(k, pos);
		if (GetRect().PointInside(pos))
			mark = false;
	}
	virtual void Event_KeyDown(SDL_Scancode code) override
	{
		TextInput.Event_KeyDown(code);
	}
	virtual void Event_KeyUp(SDL_Scancode code) override
	{
		TextInput.Event_KeyUp(code);
	}
	bool Changed()
	{
		return TextInput.Changed();
	}
	std::string GetText()
	{
		return TextInput.GetText();
	}
	void MarkRed()
	{
		mark = true;
	}
private:
	UITextInput TextInput;
	bool mark = false;
};