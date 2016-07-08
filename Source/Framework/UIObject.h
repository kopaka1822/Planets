#pragma once
#include "Input.h"
#include "Drawing.h"
#include "Sound.h"
#include <string>
#include "Framework.h"

class UIObject : public Input::EventReceiver
{
public:
	virtual void SetOrigin(const PointF& p)
	{
		pos = p;
	}
	virtual void SetCenter(const PointF& p)
	{
		SetOrigin(p - (dim / 2.0f));
	}
	virtual PointF GetOrigin() const
	{
		return pos;
	}
	virtual void SetMetrics(const PointF& d)
	{
		dim = d;
	}
	virtual PointF GetMetrics() const
	{
		return dim;
	}
	virtual void CenterX(float y)
	{
		pos.y = y;
		pos.x = (float(Framework::STD_DRAW_X) / 2.0f) - (dim.x / 2.0f);
		SetOrigin(pos);
	}
	virtual void Center()
	{
		pos.y = (float(Framework::STD_DRAW_Y) / 2.0f) - (dim.y / 2.0f);
		pos.x = (float(Framework::STD_DRAW_X) / 2.0f) - (dim.x / 2.0f);
		SetOrigin(pos);
	}

	virtual void Draw(Drawing& draw) = 0;
	virtual ~UIObject(){}
	RectF GetRect() const
	{
		return RectF(pos, pos + dim);
	}
	virtual void SetWidth(float w)
	{
		SetMetrics({ w, dim.y });
	}
	virtual void SetHeight(float h)
	{
		SetMetrics({ dim.x, h });
	}

	virtual void Enable()
	{
		enabled = true;
	}
	virtual void Disable()
	{
		enabled = false;
	}
	bool isEnabled() const
	{
		return enabled;
	}
	virtual void Register(class GameState&);
protected:
	inline PointF GetMidpoint()
	{
		return PointF(pos + (dim / 2.0f));
	}
protected:
	PointF pos;
	PointF dim;
private:
	bool enabled = true;
};