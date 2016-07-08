#pragma once
#include "UILabel.h"

// integer numbers only
class UILabelNumber : public UILabel
{
public:
	UILabelNumber(Font& font, int maxIndent)
		:
		UILabel(" ",font),
		maxIndent(maxIndent)
	{
		SetNumber(0);
		AdjustToFont();
	}
	void SetNumber(int num)
	{
		char buffer[512];
		sprintf(buffer, "%*d", maxIndent, num);
		this->num = num;

		SetText(buffer);
	}
	int GetNumber() const
	{
		return num;
	}
private:
	const int maxIndent;
	int num = 0;
};
