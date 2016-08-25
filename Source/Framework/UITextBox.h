#pragma once
#include "UIObject.h"

class UITextBox : public UIObject
{
public:
	UITextBox(const std::string& txt, Font& font)
		:
		original(txt),
		font(font),
		col(Color::White())
	{

	}
	void SetColor(Color c)
	{
		col = c;
	}
	void AdjustToFont(float width)
	{
		SplitText((int)width);
		float y = float(2 * padding + (lines.size() - 1) * padding) + font.GetFontHeight() * float(lines.size());
		setMetrics({ width, y });
	}
	void SetText(const std::string& s)
	{
		original = s;
		AdjustToFont(m_dim.x);
	}
	virtual ~UITextBox()
	{

	}
	virtual void draw(Drawing& draw) override
	{
		PointF start = m_pos + PointF(padding, padding);
		font.SetColor(col);
		LockGuard g(muTxt);
		for (const auto& l : lines)
		{
			font.Text(l, start);
			start.y += font.GetFontHeight() + padding;
		}
		g.Unlock();
	}
private:
	void SplitText(int width)
	{
		LockGuard g(muTxt);

		lines.clear();
		width = width - 2 * padding;
		size_t maxChars = width / (int)font.GetFontWidth();
		if (maxChars <= 0)
			return;

		//split original
		std::string tmp = original;

		while (tmp.size())
		{
			if (tmp.size() <= maxChars)
			{
				lines.push_back(tmp);
				tmp.clear();
			}
			else
			{
				lines.push_back(tmp.substr(0, maxChars - 1));
				tmp = tmp.substr(maxChars - 1);
			}
		}

		g.Unlock();
	}
private:
	Font& font;
	std::vector< std::string > lines;
	std::string original;
	Color col;
	Mutex muTxt;

	static const int padding = 4;
};