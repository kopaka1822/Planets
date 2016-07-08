#include "Framework.h"
#include <assert.h>

static int CLIENT_X = 0;
static int CLIENT_Y = 0;

static int CUR_DRAW_X;
static int CUR_DRAW_Y;

static float SCALAR = 1.0f;
PointF Framework::DrawStart()
{
	return PointF(float(CUR_DRAW_X - STD_DRAW_X) / -2.0f, float(CUR_DRAW_Y - STD_DRAW_Y) / -2.0f);
}
PointF Framework::DrawWidth()
{
	return PointF(CUR_DRAW_X, CUR_DRAW_Y);
}
PointF Framework::ConvertClientPoint(const PointI& cliPoint)
{
	PointF res(float(cliPoint.x) / float(CLIENT_X) * float(CUR_DRAW_X),
		float(cliPoint.y) / float(CLIENT_Y) * float(CUR_DRAW_Y));

	res.x += (float(STD_DRAW_X) - float(CUR_DRAW_X)) / 2.0f;
	res.y += (float(STD_DRAW_Y) - float(CUR_DRAW_Y)) / 2.0f;

	return res;
}
PointF Framework::Denormalize(PointF pos)
{
	const PointF st = DrawStart();
	const PointF dim = DrawWidth();
	PointF res;
	res.x = st.x + pos.x * dim.x;
	res.y = st.y + pos.y * dim.y;

	return res;
}
PointI Framework::ConvertDrawPoint(PointF drPoint)
{
	assert(CLIENT_X != 0);
	drPoint.x -= (float(STD_DRAW_X) - float(CUR_DRAW_X)) / 2.0f;
	drPoint.y -= (float(STD_DRAW_Y) - float(CUR_DRAW_Y)) / 2.0f;

	drPoint.x = drPoint.x * float(CLIENT_X) / float(CUR_DRAW_X);
	drPoint.y = drPoint.y * float(CLIENT_Y) / float(CUR_DRAW_Y);
	return drPoint;
}

void Framework::SetWindowSize(int CliWidth, int CliHeight, int WinX, int WinY, float scalar)
{
	CLIENT_X = CliWidth;
	CLIENT_Y = CliHeight;
	CUR_DRAW_X = WinX;
	CUR_DRAW_Y = WinY;
	SCALAR = scalar;
}
float Framework::GetScalar()
{
	return SCALAR;
}