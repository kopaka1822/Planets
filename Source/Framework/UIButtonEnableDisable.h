#pragma once
#include "UIButtonText.h"

class UIButtonEnableDisable : public UIButtonText
{
public:
	UIButtonEnableDisable(bool bState, Font& font)
		:
		UIButtonText("Disabled", font),
		bValue(bState)
	{
		SetValueNoChange(bState);
	}

	void SetValueNoChange(bool b)
	{
		bValue = b;
		if (bValue)
		{
			SetText("Enabled");
			SetColor(Color::Green());
		}
		else
		{
			SetText("Disabled");
			SetColor(Color::Red());
		}
	}
	void SetValue(bool b)
	{
		SetValueNoChange(b);
		bChanged = true;
	}
	bool GetState() const
	{
		return bValue;
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		UIButton::Event_MouseUp(k, pos);
		if (bClick)
			SetValue(!bValue);
	}
private:
	bool bValue;
	bool bChanged = false;
};