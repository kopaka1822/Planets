#pragma once
#include "UIButton.h"

class UIButtonLevel : public UIButton
{
public:
	UIButtonLevel(int lvl)
		:
		lvl(lvl),
		font(FONT_SMALL)
	{
		SetMetrics({ GetWdith(), GetHeight() });

		// Level format: Level 03

		name = "Level ";
		if (lvl < 10)
			name += "0";

		name += std::to_string(lvl);

		PointF fontMetric = font.GetMetrics(name);
		fontStart.x = (dim.x - fontMetric.x) / 2.0f;
		fontStart.y = 14.0f;

		// StarStart
		starStart.y = 50.0f;
		float starwi = 3.0f * StarWidth + 2.0f * StartPadd;
		starStart.x = (dim.x - starwi) / 2.0f;
	}

	virtual void Draw(Drawing& draw) override
	{
		if (isEnabled())
			UIButton::Draw(draw);
		else
			draw.DrawBox(GetRect(), (float)border, Color::White(), Color(30, 30, 30));
		// draw text
		if (bDown)
			font.SetColor(Color::Black());
		else
			font.SetColor(Color::White());

		font.Text(name, pos + fontStart);

		// draw stars
		PointF cur = starStart;
		const PointF met = PointF(StarWidth, StarWidth);
		for (int i = 0; i < 3; i++)
		{
			draw.DrawSprite(RectF(pos + cur, pos + cur + met), GetStarTex(i));
			cur.x += StarWidth + StartPadd;
		}
	}

	static float GetWdith()
	{
		return 200.0f;
	}
	static float GetHeight()
	{
		return 110.0f;
	}
	void SetStars(bool s1, bool s2, bool s3)
	{
		startflags = 0;
		if (s1)
			startflags |= 1;
		if (s2)
			startflags |= 2;
		if (s3)
			startflags |= 4;
	}
	void SetStarFlag(int starflag)
	{
		startflags = starflag;
	}
private:
	// number = 0, 1, 2
	Texture& GetStarTex(int number)
	{
		if (startflags & (1 << number))
			return Database::GetTexture(Database::GameTex::StartFilled);
		else
			return Database::GetTexture(Database::GameTex::StarEmpty);
	}
private:
	const int lvl;
	Font& font;

	std::string name; // Level 04
	PointF fontStart;
	PointF starStart;
	int startflags = 0;

	static const int StarWidth = 40;
	static const int StartPadd = 8;
};