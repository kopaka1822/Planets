#pragma once
#include "UIObject.h"
#include "../Game/Map.h"
#include "MapDrawer.h"

class MiniMap : public UIObject
{
public:
	MiniMap(Map& map, MapDrawer& drawer)
		:
		map(map), drawer(drawer),
		mapWi(map.GetWidth()), mapHi(map.GetHeight())
	{}
	virtual void draw(Drawing& draw) override
	{
		draw.DrawBox(getRect(), (float)border, Color::White(), Color::Black());

		draw.DrawBox(RectF(m_pos + mStart, m_pos + mEnd),1.0f,Color::Blue(), Color::Black());

		for (const auto& p : map.plans)
		{
			draw.DrawPlanet(fromModel(p->GetPos()), Color::GetTeamColor(p->GetTeam()), fromModel(p->GetRadius()));
		}

		//draw Cam Rect
		RectF ownRect = getRect();
		ownRect.x1 += border;
		ownRect.y1 += border;
		ownRect.x2 -= border;
		ownRect.y2 -= border;

		RectF dstRect = fromModel(drawer.GetCamRectSmooth()).ClipTo(ownRect);
		if (ownRect.RectFullInside(dstRect))
			draw.DrawTransBox(dstRect, 2.0f, Color::Yellow());
		//draw.DrawTransBox(fromModel(drawer.GetCamRect()), 2.0f, Color::Yellow());
	}
	virtual void setMetrics(const PointF& m) override
	{
		UIObject::setMetrics(m);

		float wi = m.x - 2 * float(border);
		float hi = m.y - 2 * float(border);

		float asp = wi / hi;

		float mWi = mapWi;
		float mHi = mapHi;

		float mAsp = mWi / mHi;

		if (asp == mAsp)
		{
			mStart = PointF(border, border);
			mEnd = m_dim - PointF(border, border);
		}
		else if (asp > mAsp)
		{
			mStart.y = (float)border;
			mEnd.y = m_dim.y - border;

			float w = mAsp / asp * (wi - 2 * border);
			mStart.x = border + (wi - w) / 2.0f;
			mEnd.x = m_dim.y - mStart.x;
		}
		else
		{
			mStart.x = (float)border;
			mEnd.x = m_dim.x - border;

			float h = asp / mAsp * (hi - 2 * border);
			mStart.y = border + (hi - h) / 2.0f;
			mEnd.y = m_dim.y - mStart.y;
		}

		scalar = std::min((mEnd.x - mStart.x) / mapWi,
			(mEnd.y - mStart.y) / mapHi);
	}

private:
	inline PointF fromModel(PointF p)
	{
		PointF dim = mEnd - mStart;
		p.x = p.x / mapWi * dim.x;
		p.y = p.y / mapHi * dim.y;
		return p + mStart + m_pos;
	}
	inline float fromModel(float f)
	{
		return scalar * f;
	}
	inline RectF fromModel(RectF r)
	{
		r.p1 = fromModel(r.p1);
		r.p2 = fromModel(r.p2);
		return r;
	}
	PointF toModel(PointF p)
	{
		PointF dim = mEnd - mStart;
		p = p - m_pos - mStart;
		p.x = p.x * mapWi / dim.x;
		p.y = p.y * mapHi / dim.y;
		return p;
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		if (getRect().PointInside(pos))
		{
			bDown = true;
			Event_MouseMove(pos);
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		bDown = false;
	}
	virtual void Event_MouseMove(const PointF& p) override
	{
		if (bDown)
			drawer.SetCam(toModel(p));
	}
private:
	Map& map;
	MapDrawer& drawer;
	const int border = 2;
	
	PointF mStart; //relative to pos
	PointF mEnd;

	const float mapWi;
	const float mapHi;
	float scalar;

	//Mouse vars
	bool bDown = false;
};