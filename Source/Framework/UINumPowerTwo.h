#pragma once
#include "UINumUpDownInt.h"

class UINumPowerTwo : public UINumUpDownInt
{
public:
	UINumPowerTwo(int start,int min, int max, Font& font)
		:
		UINumUpDownInt(start,min,max,1,font)
	{

	}
	virtual bool SetValueNoChange(int val) override
	{
		if (val == value)
			return false;

		int prev = value;
		bool allow = false;
		if (val <= 0)
		{
			allow = true;
			val = 0;
		}
		if (val >= ma)
		{
			allow = true;
			val = ma;
		}
		if (!allow)
		{
			if (val < prev)
			{
				// round down
				for (int i = ma; i > 0; i /= 2)
				{
					if (val >= i)
					{
						val = i;
						break;
					}
				}
			}
			else
			{
				// round up
				for (int i = 1; i <= ma; i *= 2)
				{
					if (val <= i)
					{
						val = i;
						break;
					}
				}
			}
		}

		// change?
		bChange = false;
		value = val;
		return value != prev;
	}
};