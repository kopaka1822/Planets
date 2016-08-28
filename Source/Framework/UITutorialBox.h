#pragma once
#include "UIObject.h"

class UITutorialBox : public UIObject
{
public:
	UITutorialBox()
		:
		font(FONT_MED)
	{}
	void SetTexture(const Texture* pTexture)
	{
		pTex = pTexture;
	}
	void AddLine(std::string s)
	{
		lines.push_back(s);
	}
	void EnablePlanet(Color col)
	{
		bPlan = true;
		planCol = col;
	}
	// call after settings metrics and origin
	void OrderItems()
	{
		float totalHeight = 0.0f;
		for (const auto& s : lines)
		{
			totalHeight += font.GetFontHeight() + padding;
		}
		if (pTex || bPlan)
			totalHeight += 200.0f + padding;

		PointF curPos = m_pos + PointF(border + padding, border + padding);
		curPos.y += (m_dim.y - 2 * (border + padding) - totalHeight) / 2.0f;

		if (pTex)
		{
			// height -> 200
			texturePos.y = curPos.y;
			textureDim.y = 200.0f;
			//textureDim.x = pTex->GetHeight() / textureDim.y * pTex->GetWidth();
			textureDim.x = textureDim.y / pTex->GetHeight() * pTex->GetWidth();
			texturePos.x = m_pos.x + (m_dim.x - textureDim.x) / 2.0f;

			curPos.y += textureDim.y + padding;
		}
		else if (bPlan)
		{
			texturePos.x = m_pos.x + m_dim.x / 2.0f;
			texturePos.y = curPos.y + 100.0f;

			curPos.y += 200.0f + padding;
		}

		fontPos.clear();
		for (const auto& s : lines)
		{
			PointF fmet = font.GetMetrics(s);

			PointF fpos;
			fpos.y = curPos.y;
			fpos.x = m_pos.x + m_dim.x /2.0f - fmet.x / 2.0f;

			fontPos.push_back(fpos);

			curPos.y += fmet.y + padding;
		}
	}
	virtual void draw(Drawing& draw) override
	{ 
		if (!bActive)
			return;

		draw.DrawBox(getRect(), border, Color::Red().Brightness(0.2f), Color::Black());

		if (pTex)
		{
			draw.DrawSprite(RectF(texturePos, texturePos + textureDim), *pTex);
		}
		else if (bPlan)
		{
			draw.DrawPlanet(texturePos, planCol, 100.0f);
		}

		font.SetColor(Color::White());
		
		for (size_t i = 0; i < lines.size(); i++)
		{
			font.Text(lines[i], fontPos[i]);
		}
	}
	virtual void Event_MouseDown(Input::MouseKey k, const PointF& p) override
	{
		if (bActive && k == Input::Left)
		{
			bActive = false;
			bDone = true;
			if (pNext)
				pNext->Activate();
		}
	}
	void RegisterNext(UITutorialBox* p)
	{
		pNext = p;
	}
	void Activate()
	{
		if (!bDone)
			bActive = true;
	}
	bool isActive() const
	{
		return bActive;
	}
	bool isDone() const
	{
		return bDone;
	}
private:
	bool bActive = false;
	bool bDone = false;

	Font& font;

	const Texture* pTex = nullptr;
	bool bPlan = false;
	Color planCol;
	std::vector< std::string > lines;
	std::vector< PointF > fontPos;
	PointF texturePos;
	PointF textureDim;

	UITutorialBox* pNext = nullptr;

	const float border = 6.0f;
	const float padding = 4.0f;
};