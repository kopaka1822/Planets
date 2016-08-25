#pragma once
#include "UIObject.h"
#include "../Utility/Mutex.h"

class UILabel : public UIObject
{
public:
	UILabel(const std::string& txt, Font& font)
		:
		txt(txt),
		font(font),
		col(Color::White())
	{
		AdjustToFont();
	}
	virtual void draw(Drawing& draw)
	{
		font.SetColor(col);
		muTxt.Lock();
		font.Text(txt, m_pos);
		muTxt.Unlock();
	}
	void AdjustToFont()
	{
		/*dim.x = font.GetFontWidth() * txt.length();
		dim.y = font.GetFontHeight();*/
		m_dim = font.GetMetrics(txt);
	}
	std::string GetText() const
	{
		return txt;
	}
	virtual void SetText(const std::string& str)
	{
		muTxt.Lock();
		txt = str;
		muTxt.Unlock();
	}
	void SetColor(Color c)
	{
		col = c;
	}
protected:
	std::string txt;
	Mutex muTxt;
	Font& font;
	Color col;
};
