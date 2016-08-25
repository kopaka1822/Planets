#pragma once
#include "UIButtonText.h"
#include "../Utility/DialogOpenFile.h"

class UIButtonFile : public UIButtonText
{
public:
	UIButtonFile(Font& font)
		:
		UIButtonText("...",font)
	{}
	void AdjustToFont(float width)
	{
		setMetrics({ width, m_dim.y });
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		UIButton::Event_MouseUp(k, pos);

		if (Pushed())
		{
			// Open File Dialog and change filename
			DialogOpenFile dia("map");
			dia.Show();

			if (dia.IsSuccess())
			{
				SetPath(dia.GetName());
			}
		}
	}
	std::string GetPath() const
	{
		return fullpath;
	}
	void SetPath(const std::string& p)
	{
		fullpath = p;
		// cut c:\..\level1.map to level1
		if (p.length())
			SetText(tool::fileRemovePath(fullpath));
		else
			SetText("...");
	}
protected:
	std::string fullpath; // full filepath (not the display name)
};