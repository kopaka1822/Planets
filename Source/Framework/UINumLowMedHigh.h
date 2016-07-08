#pragma once
#include "UINumUpDown.h"

class UINumLowMedHigh : public UINumUpDown<int>
{
public:
	enum VALUE
	{
		LOW,
		MEDIUM,
		HIGH
	};
public:
	UINumLowMedHigh(VALUE start, Font& font)
		:
		UINumUpDown(start, LOW, HIGH, 1, font)
	{
		input.SetFlags(UITextInput::F_LOWERCASE);
	}
protected:
	virtual void DrawNum(Font& f, const PointF& start) const override
	{
		f.Text(CutString(NumToString()), start);
	}
	virtual std::string NumToString() const override
	{
		switch (value)
		{
		case LOW:
			return "low";
			break;
		case MEDIUM:
			return "medium";
			break;
		case HIGH:
			return "high";
			break;
		default:
			return "unknown";
		}
	}
	virtual int StringToNum(const std::string& s) override
	{
		if (s == "medium")
			return MEDIUM;
		else if (s == "high")
			return HIGH;
		else
			return LOW;
	}
};