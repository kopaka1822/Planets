#pragma once
#include "UINumUpDown.h"

class UINumUpDownInt : public UINumUpDown<int>
{
public:
	UINumUpDownInt(int start, int min, int max, int step, Font& font)
		:
		UINumUpDown(start,min,max,step,font)
	{
		input.SetFlags(UITextInput::F_NUMBER);
	}
protected:
	virtual void DrawNum(Font& f, const PointF& start) const override
	{
		f.Text(CutString(NumToString()), start);
	}
	virtual std::string NumToString() const override
	{
		return std::to_string((int)value);
	}
	virtual int StringToNum(const std::string& s) override
	{
		if (s.length() == 0)
			return 0;

		return atoi(s.c_str());
	}
};

class UINumUpDownFloat : public UINumUpDown<float>
{
public:
	UINumUpDownFloat(float start, float min, float max, float step, Font& font)
		:
		UINumUpDown(start, min, max, step, font)
	{
		input.SetFlags(UITextInput::F_FLOAT);
	}
protected:
	virtual void DrawNum(Font& f, const PointF& start) const override
	{
		
		f.Text(CutString(NumToString()), start);
	}
	virtual std::string NumToString() const override
	{
		return std::to_string((float)value);
	}
	virtual float StringToNum(const std::string& s) override
	{
		if (s.length() == 0)
			return 0.0f;

		return (float)atof(s.c_str());
	}
};

