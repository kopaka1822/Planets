#pragma once
#include "Input.h"
#include "Drawing.h"
#include "Sound.h"
#include <string>
#include "Framework.h"

class UIObject : public Input::EventReceiver
{
public:
	virtual void setOrigin(const PointF& p)
	{
		m_pos = p;
	}
	virtual void setCenter(const PointF& p)
	{
		setOrigin(p - (m_dim / 2.0f));
	}
	virtual PointF getOrigin() const
	{
		return m_pos;
	}
	virtual void setMetrics(const PointF& d)
	{
		m_dim = d;
	}
	virtual PointF getMetrics() const
	{
		return m_dim;
	}
	virtual void centerX(float y)
	{
		m_pos.y = y;
		m_pos.x = (float(Framework::STD_DRAW_X) / 2.0f) - (m_dim.x / 2.0f);
		setOrigin(m_pos);
	}
	virtual void center()
	{
		m_pos.y = (float(Framework::STD_DRAW_Y) / 2.0f) - (m_dim.y / 2.0f);
		m_pos.x = (float(Framework::STD_DRAW_X) / 2.0f) - (m_dim.x / 2.0f);
		setOrigin(m_pos);
	}

	virtual void draw(Drawing& draw) = 0;
	virtual ~UIObject(){}
	RectF getRect() const
	{
		return RectF(m_pos, m_pos + m_dim);
	}
	virtual void setWidth(float w)
	{
		setMetrics({ w, m_dim.y });
	}
	virtual void setHeight(float h)
	{
		setMetrics({ m_dim.x, h });
	}

	virtual void enable()
	{
		m_isEnabled = true;
	}
	virtual void disable()
	{
		m_isEnabled = false;
	}
	bool isEnabled() const
	{
		return m_isEnabled;
	}
	virtual void registerMe(class GameState&);
protected:
	inline PointF getMidpoint()
	{
		return PointF(m_pos + (m_dim / 2.0f));
	}
protected:
	PointF m_pos;
	PointF m_dim;
private:
	bool m_isEnabled = true;
};