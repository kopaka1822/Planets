#pragma once
#include "../Utility/Point.h"

class Framework
{
public:
	static void SetWindowSize(int CliWidth, int CliHeight, int WinX, int WinY, float scalar);

	static PointF ConvertClientPoint(const PointI& clPoint); // client to draw area  (Mouse pos to draw pos)
	static PointI ConvertDrawPoint(PointF drPoint); // draw area to client

	static PointF Denormalize(PointF pos); // 0..1
	static float GetScalar();

	static PointF DrawStart();
	static PointF DrawWidth();

	static const int STD_DRAW_X = 1280;
	static const int STD_DRAW_Y = 720;

	static const unsigned int SIZE_MIN_X = 800;
	static const unsigned int SIZE_MIN_Y = 600;
};