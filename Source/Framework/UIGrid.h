#pragma once

#include "UIObject.h"
#include "UIButtonText.h"
#include <vector>

class UIGrid : public UIObject
{
public:
	UIGrid(Font& font, int amount, int rows, bool box)
		:
		Amount(amount),
		Rows(rows),
		bBox(box)
	{
		btns.assign(Amount, nullptr);
		for (unsigned int i = 0; i < btns.size(); i++)
		{
			if (i < 9)
				btns[i] = new UIButtonText("Level 0" + std::to_string(i + 1), font);
			else
				btns[i] = new UIButtonText("Level " + std::to_string(i + 1), font);
		}
	}
	~UIGrid()
	{
		for (auto& i : btns)
		{
			delete i;
			i = nullptr;
		}
	}
	void SetPosition()
	{
		for (unsigned int i = 0; i < btns.size(); i++)
		{
			btns[i]->SetOrigin(PointF((int)(i / Rows) * btns[0]->GetMetrics().x + (int)(i / Rows) * 10, btns[0]->GetMetrics().y * (i % Rows) + 10 * (i % Rows) ) + pos);
		}
	}
	virtual void Draw(Drawing& draw) override
	{
		if (bBox)
		{
			draw.DrawBox(RectF(btns[0]->GetOrigin() - PointF(2.0f * border, 2.0f * border), PointF(btns[Amount - 1]->GetOrigin().x + btns[Amount - 1]->GetMetrics().x + 2.0f * border, btns[Rows-1]->GetOrigin().y + btns[Rows-1]->GetMetrics().y + 2.0f *border)), (float)border, Color::White(), Color::Black());
		}
		for (unsigned int i = 0; i < btns.size(); i++)
		{
			btns[i]->Draw(draw);
		}
	}
	UIButtonText* GetPointer(int i)
	{
		if (i >= 0 && i < GetSize())
			return btns[i];
		else
			return nullptr;
	}
	int GetSize()
	{
		return btns.size();
	}
	virtual void SetOrigin(const PointF& p) override
	{
		UIObject::SetOrigin(p);
		SetPosition();
	}

private:
	std::vector<UIButtonText*> btns;

	int Amount;
	int Rows;
	bool bBox;
	static const int border = 6;
};