#pragma once
#include "Rect.h"
#include <assert.h>

template <class T>
class _Line //endless Line
{
public:
	_Line(const _Point<T>& p1,const _Point<T>& p2)
	{
		a = p1;
		b = p2;
		vABnorm = (p2 - p1).normalize();
	}
	virtual ~_Line(){}
	inline _Point<T> vecNorm() const
	{
		return vABnorm;
	}
	inline _Point<T> closestPoint(_Point<T> pt) const
	{
		return GetPoint(closestPointScalar(pt));
	}
	inline virtual T closestPointScalar(_Point<T> pt) const
	{
		return (pt - a) * vABnorm;
	}
	inline T intersectCircle(const _Point<T>& center, const T radius) const//returns scalar |negative value = no intersection
	{
		const T r2 = radius * radius;
		T scaleClose = closestPointScalar(center);
		T dist2 = (GetPoint(scaleClose) - center).lengthSq(); //distance between closest point and center (squared)
		if (dist2 > r2)
		{
			//no intersection
			return (T)-1.0;
		}
		//intersection
		T scale = sqrt(r2 - dist2);

		return scaleClose - scale;
	}
	inline _Point<T> GetPoint(T scalar) const
	{
		return a + vABnorm * scalar;
	}
	enum side
	{
		left,
		right
	};
	inline side whichSide(const _Point<T>& pt) const
	{
		_Vector<T> vAP = (pt - a).normalize();
		if (vABnorm.cross(vAP) > 0)
		{
			return side::left;
		}
		else
		{
			return side::right;
		}
	}
	enum direc
	{
		towardLine,
		awayFromLine
	};
	inline direc vectorDir(const _Point<T>& pos, const _Point<T>& dir) const
	{
		if (whichSide(pos) == side::left)
		{
			if (vABnorm.CW90() * dir > 0)
				return direc::towardLine;
			else
				return direc::awayFromLine;
		}
		else
		{
			if (vABnorm.CW90() * dir < 0)
				return direc::towardLine;
			else
				return direc::awayFromLine;
		}
	}

	inline virtual bool intersect(const _Line<T>& other, const float maxDistance) const
	{
		//parallel?
		if (a.isParallel(other.a))
		{
			//identical?
			if (other.a.isClose(closestPoint(other.a), maxDistance))
				return true;
			else
				return false;
		}
		//not parallel -> intersection
		return true;
	}
protected:
	_Point<T> a; //start
	_Point<T> b; //end
	_Vector<T> vABnorm;
};

using LineF = _Line< float >;
using LineI = _Line< int >;

template <class T>
class _LineFixed : public _Line<T>
{
	_Vector<T> vAB;
public:
	_LineFixed(const _Point<T>& p1, const _Point<T>& p2)
		:
		_Line<T>(p1,p2)
	{
		vAB = p2 - p1;
	}


	//new functions
	inline const _Point<T>& P1() const
	{
		return this->a;
	}
	inline const _Point<T>& P2() const
	{
		return this->b;
	}
	inline const _Point<T>& Vector() const
	{
		return vAB;
	}

	//helper functions
	inline T lengthSq() const
	{
		return vAB.lengthSq();
	}
	inline T length() const
	{
		return vAB.length();
	}

	inline virtual T closestPointScalar(_Point<T> pt) const override
	{
		T scalar = _Line<T>::closestPointScalar(pt);
		if (scalar <= 0)
			return 0;

		if (this->vABnorm * scalar >= vAB)
			return vAB.length();

		return scalar;
	}
};

using LineFixedF = _LineFixed< float >;
using LineFixedI = _LineFixed< int >;

template <class T>
class _LineFixedOne : public _Line<T> //one end is fixed, the other endless
{
public:
	_LineFixedOne(const _Point<T>& fixedPt, const _Point<T>& pt2)
		:
		_Line<T>(fixedPt, pt2)
	{

	}
	//new functions
	inline const _Point<T>& P1() const
	{
		return _Line<T>::a;
	}

	inline virtual T closestPointScalar(_Point<T> pt) const override
	{
		T scalar = _Line<T>::closestPointScalar(pt);
		if (scalar <= 0)
			return 0;

		return scalar;
	}
	inline virtual bool intersect(const _Line<T>& other, const float maxDistance) const override
	{
		//intersection if facing onwards or really close
		if (other.vectorDir(_Line<T>::a, _Line<T>::vABnorm) == _Line<T>::direc::towardLine)
			return true;

		//close to the start point
		if (this->a.isClose(other.closestPoint(this->a), maxDistance))
			return true;

		return false;
	}
};

using LineFixedOneF = _LineFixedOne< float >;
using LineFixedOneI = _LineFixedOne< int >;
