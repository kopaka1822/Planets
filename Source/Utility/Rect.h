#pragma once
#include "Point.h"
#include <algorithm>
#include <vector>

template <class T>
class _Rect
{
public:
	union
	{
		struct
		{
			T
				x1,
				y1,
				x2,
				y2;
		};
		struct
		{
			T
				left,
				top,
				right,
				bottom;
		};

		struct
		{
			_Point<T> p1; //top left
			_Point<T> p2; //bottom right
		};
	};
	_Rect()
	{
		_Rect((T)0.0, (T)0.0, (T)0.0, (T)0.0);
	}
	_Rect(T left, T top, T right, T bottom)
		:
		left(left), right(right), top(top), bottom(bottom)
	{}
	_Rect(const _Point<T>& po1,const _Point<T>& po2)
		:
		x1(std::min(po1.x,po2.x)),
		x2(std::max(po1.x, po2.x)),
		y1(std::min(po1.y, po2.y)),
		y2(std::max(po1.y, po2.y))
	{}
	template <class T2>
	_Rect<T>(const _Rect<T2>& copy)
		:
		_Rect((T)copy.x1,(T)copy.y1,(T)copy.x2,(T)copy.y2)
	{
		
	}
	//operator
	inline _Rect<T>& operator=(const _Rect<T>& rhs)
	{
		x1 = rhs.x1;
		x2 = rhs.x2;
		y1 = rhs.y1;
		y2 = rhs.y2;
		return *this;
	}
	inline _Rect<T>& operator+=(const _Point<T>& pt)
	{
		p1 += pt;
		p2 += pt;
		return *this;
	}
	inline _Rect<T> operator+(const _Point<T>& pt) const
	{
		_Rect<T> t = *this;
		t += pt;
		return t;
	}
	inline _Rect<T>& operator-=(const _Point<T>& pt)
	{
		p1 -= pt;
		p2 -= pt;
		return *this;
	}
	inline _Rect<T> operator-(const _Point<T>& pt) const
	{ 
		_Rect<T> t = *this;
		t -= pt;
		return t;
	}

	//other
	inline T getWidth() const
	{
		return (x2 - x1);
	}
	inline T getHeight() const
	{
		return (y2 - y1);
	}

	//functions
	inline bool PointInside(const _Point<T>& pt) const
	{
		if (pt.x >= left)
			if (pt.x <= right)
				if (pt.y >= top)
					if (pt.y <= bottom)
						return true;
		return false;
	}
	inline bool RectFullInside(const _Rect<T>& re) const
	{
		if (PointInside(re.TopLeft()))
			if (PointInside(re.BottomRight()))
				if (PointInside(re.TopRight()))
					if (PointInside(re.BottomLeft()))
						return true;
		return false;
	}
	inline bool RectCutting(const  _Rect<T>& re) const
	{
		if (PointInside(re.TopLeft()) || PointInside(re.BottomRight()) || 
			PointInside(re.TopRight()) || PointInside(re.BottomLeft()))
			return true;
		return false;
	}
	//clip this rect into another
	inline _Rect<T> ClipTo(const _Rect<T>& o)
	{
		_Rect<T> r;
		r.x1 = std::max(o.x1, x1);
		r.x2 = std::min(o.x2, x2);

		r.y1 = std::max(o.y1, y1);
		r.y2 = std::min(o.y2, y2);

		r.x1 = std::min(r.x1, r.x2);
		r.y1 = std::min(r.y1, r.y2);

		r.x2 = std::min(r.x2, x2);
		r.y2 = std::min(r.y2, y2);

		return r;
	}
	// vector with 4 points (topleft, topright...)
	inline std::vector<_Point<T>> GetVector() const
	{
		std::vector<_Point<T>> vec;

		vec.push_back(TopLeft());
		vec.push_back(TopRight());
		vec.push_back(BottomRight());
		vec.push_back(BottomLeft());

		return vec;
	}
	inline _Point<T> GetMetrics() const
	{
		return _Point<T>(getWidth(), getHeight());
	}
	inline _Point<T> TopRight() const
	{
		return _Point<T>(x2, y1);
	}
	inline _Point<T> TopLeft() const
	{
		return p1;
	}
	inline _Point<T> BottomRight() const
	{
		return p2;
	}
	inline _Point<T> BottomLeft() const
	{
		return _Point<T>(x1, y2);
	}
	inline _Point<T> GetMidpoint() const
	{
		return (p1 + p2) / 2.0f;
	}
	inline _Point<T> Midpoint() const
	{
		return _Point<T>((x1 + x2) / (T)2.0, (y1 + y2) / (T)2.0);
	}
	static _Rect<T> FromPoint(const _Point<T>& p, T radius)
	{
		return _Rect<T>(p.x - radius, p.y - radius, p.x + radius, p.y + radius);
	}
};

typedef _Rect< float > RectF;
typedef _Rect< int > RectI;