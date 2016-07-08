#pragma once
#include "UINumUpDown.h"
#include "../Game/MapLoader.h"

class UINumEntityType : public UINumUpDown<int>
{
public:
	UINumEntityType(Font& font)
		:
		UINumUpDown(0, 0, 3, 1, font)
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
		// enum to string
		switch (value)
		{
		case MapLoader::E_NORMAL:
			return "entity";
			break;
		case MapLoader::E_BOMBER:
			return "bomber";
			break;
		case MapLoader::E_SPEEDER:
			return "speeder";
			break;
		case MapLoader::E_SABOTEUR:
			return "saboteur";
			break;
		default:
			return "error";
		}
	}
	virtual int StringToNum(const std::string& s) override
	{
		if (s.length() == 0)
			return 0;

		if (s == "entity")
			return MapLoader::E_NORMAL;

		if (s == "bomber")
			return MapLoader::E_BOMBER;

		if (s == "speeder")
			return MapLoader::E_SPEEDER;

		if (s == "saboteur")
			return MapLoader::E_SABOTEUR;

		return -1;
	}
};