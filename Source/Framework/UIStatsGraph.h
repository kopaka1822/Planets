#pragma once
#include "UIObject.h"

class UIStatsGraph : public UIObject
{
public:
	UIStatsGraph(const std::vector< std::vector< unsigned int >>& data, int myTeam, Font& font)
		:
		gData(data),
		myIndex(myTeam - 1),
		maxTeams(data[0].size()),
		font(font)
	{
		for (const auto& vec : gData)
		{
			for (const auto& i : vec)
			{
				maxUnit = std::max(maxUnit, i);
			}
		}

		// calc number bar width
		numberBarWidth = fontpadd * 2 + border + font.GetFontWidth() * 6.0f;
	}
	virtual ~UIStatsGraph(){}

	virtual void draw(Drawing& draw) override
	{
		// draw numbers

		font.SetColor(Color::White());
		float x = m_pos.y + border + fontpadd;
		float curY = m_pos.y + m_dim.y - border - 2.0f * font.GetFontHeight();

		float yFac = (m_dim.y - 2.0f * border) / float(maxUnit);
		while (curY > m_pos.y + border)
		{
			// get number
			float y = m_dim.y - ((curY + font.GetFontHeight() / 2.0f) - m_pos.y);
			size_t count = size_t(y / yFac);

			font.Text(std::to_string(count), PointF(x, curY));

			// draw horizontal line
			draw.DrawHLine(PointF(m_pos.x + numberBarWidth + 1.0f, curY + font.GetFontHeight() / 2.0f), m_dim.x - numberBarWidth - 2.0f, 2.0f, Color::Gray());


			curY -= 2.0f * font.GetFontHeight();
		}


		// draw lines

		unsigned int ucount = (unsigned int)(drawPercent * gData.size());

		for (int i = 0; i < maxTeams; i++)
		{
			if (i == myIndex)
				continue;

			DrawLine(i, Color::GetTeamColor(i + 1), draw, ucount);
		}

		DrawLine(myIndex, Color::GetTeamColor(myIndex + 1), draw, ucount);

		draw.DrawVLine(m_pos + PointF(numberBarWidth + border / 2.0f, border / 2.0f), m_dim.y - border, border, Color::White());
		draw.DrawTransBox(getRect(), border, Color::White());
	}
	void Update(float dt)
	{
		drawPercent += dt / 1.0f; // 1 seconds to draw
	}
private:
	void DrawLine(int team,const Color& col, Drawing& draw, unsigned int maxUnits)
	{
		float step = (m_dim.x - 2.0f * border - numberBarWidth) / (gData.size() - 1);

		float x = numberBarWidth;

		float prevY = 0.0f;

		float yFac = (m_dim.y - 2.0f * border) / float(maxUnit);
		unsigned int unitCount = 0;

		for (const auto& vec : gData)
		{
			unsigned int count = vec[team];

			float y = yFac * float(count);

			if (x != numberBarWidth) // dont draw the point...
			{
				draw.DrawLine(m_pos + PointF(border + x - step, m_dim.y - border - prevY), m_pos + PointF(border + x, m_dim.y - border - y), col, 3.0f);
			}

			prevY = y;
			x += step;
			if (++unitCount > maxUnits)
				break;
		}
	}
private:
	Font& font;
	const std::vector< std::vector< unsigned int >>& gData;
	unsigned int maxUnit = 1;
	const int myIndex;
	const int maxTeams;

	const float border = 5.0f;
	const float fontpadd = 5.0f;
	float drawPercent = 0.0f;

	float numberBarWidth = 0.0f;
};