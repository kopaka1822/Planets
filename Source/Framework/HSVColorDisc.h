#pragma once
#include "DiffuseColor.h"
#include "Color.h"
#define _USE_MATH_DEFINES
#include <math.h>

class HSVColorDisc : public DiffuseColor
{
public:
	HSVColorDisc(unsigned int samples)
		:
		DiffuseColor(samples + 3, Color::White().col)
	{
		// generate hsv colors
		float step = (2.0f * float(M_PI)) / float(samples);
		float curAngle = 0.0f;

		for (unsigned int i = 0; i <= samples + 1; ++i)
		{
			// change pVertex[i+1]
			pVertex[i + 1] = PickHSV(curAngle).col;
			curAngle += step;
		}
	}

public:
	static Color PickHSV(float angle)
	{
		static const float pi2 = 2.0f * float(M_PI);
		
		
		Color col;
		col.a = 255;
		float h = angle / pi2;
		if (h >= 1.0f)
			h -= 1.0f;
		h = max(0.0f, h);

		col.r = (BYTE)(max(0.0f, min(1.0f, fabs(6.0f * h - 3.0f) - 1.0f)) * 255.0f);
		col.g = (BYTE)(max(0.0f, min(1.0f, 2.0f - fabs(6.0f * h - 2.0f))) * 255.0f);
		col.b = (BYTE)(max(0.0f, min(1.0f, 2.0f - fabs(6.0f * h - 4.0f))) * 255.0f);

		return col;
	}
};