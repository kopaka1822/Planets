#pragma once
#include "Settings.h"
#include "../Utility/Tools.h"
#include "../Game/GameTypes.h"

class Color
{
public:
	float  r, a, g, b;

public:
	Color()
	{}
	Color(int dword)
	{
		b = (dword & 0xFF) / 255.0f;
		g = ((dword & 0xFF00) >> 8) / 255.0f;
		r = ((dword & 0xFF0000) >> 16) / 255.0f;
		//a = ((dword) >> 24) / 255.0f;
		a = 1.0f;
	}
	Color(float r, float g, float b, float a)
		:
		r(r), g(g), b(b), a(a)
	{}
	Color(unsigned char r, unsigned char g, unsigned char b)
		:
		r(r / 255.0f), g(g / 255.0f), b(b / 255.0f), a(1.0f)
	{}

	Color mix(const Color& o) const
	{
		Color c;
		c.r = (r + o.r) / 2.0f;
		c.g = (g + o.g) / 2.0f;
		c.b = (b + o.b) / 2.0f;
		c.a = (a + o.a) / 2.0f;
		return c;
	}
	Color Brightness(float percent) const
	{
		Color c(*this);
		c.r = tool::clamp(percent * c.r, 0.0f, 1.0f);
		c.g = tool::clamp(percent * c.g, 0.0f, 1.0f);
		c.b = tool::clamp(percent * c.b, 0.0f, 1.0f);
		//c.a = tool::clamp(percent * c.a, 0.0f, 1.0f);

		return c;
	}
	Color mix(const Color& o, float op)
	{
		Color c;
		float ot = 1.0f - op;
		c.r = r * op + o.r * ot;
		c.g = g * op + o.g * ot;
		c.b = b * op + o.b * ot;
		c.a = a * op + o.a * ot;
		return c;
	}
	//statics
	static Color GetTeamColor(TeamID team)
	{
		return Settings::GetColor(team);
	}

	static Color Red()
	{
		return Color(255, 0, 0);
	}
	static Color DarkRed()
	{
		return Color(120, 0, 0);
	}
	static Color Green()
	{
		return Color(0, 255, 0);
	}
	static Color DarkGreen()
	{
		return Color(0, 120, 0);
	}
	static Color Blue()
	{
		return Color(0, 0, 255);
	}
	static Color White()
	{
		return Color(255, 255, 255);
	}
	static Color Black()
	{
		return Color(0, 0, 0);
	}
	static Color Yellow()
	{
		return Color(255, 255, 0);
	}
	static Color Zyan()
	{
		return Color(0, 255, 255);
	}
	static Color Magenta()
	{
		return Color(255, 0, 255);
	}
	static Color Gray()
	{
		return Color(100, 100, 100);
	}
};
