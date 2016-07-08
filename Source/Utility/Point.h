#pragma once
#include <math.h>

template < class T >
class _Point
{
public:
	T x, y;
	_Point(T x, T y)
		: x(x), y(y)
	{}
	_Point()
	{
		_Point((T)0.0,(T) 0.0);
	}
	template < class T2, class T3>
	_Point(T2 x, T3 y)
		:
		x((T)x),
		y((T)y)
	{}
	template <class T2>
	_Point(const _Point<T2>& copy)
	{
		x = (T)copy.x;
		y = (T)copy.y;
	}
	//operators
	inline bool operator==(const _Point<T>& rhs) const
	{
		if (x == rhs.x)
			if (y == rhs.y)
				return true;
		return false;
	}
	inline bool operator!=(const _Point<T>& rhs) const
	{
		return !(*this == rhs);
	}
	inline bool isEqual(const _Point<T>& other, T tolerance)
	{
		if (x >= other.x - tolerance)
			if (x <= other.x + tolerance)
				if (y >= other.y - tolerance)
					if (y <= other.y + tolerance)
						return true;
		return false;
	}
	template <class T2>
	inline _Point<T>& operator=(const _Point<T2>& rhs)
	{
		x = (T)rhs.x;
		y = (T)rhs.y;
		return *this;
	}
	inline _Point<T>& operator=(const _Point<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	inline _Point<T>& operator+=(const _Point<T>& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	inline _Point<T>& operator-=(const _Point<T>& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	inline _Point<T> operator+(const _Point<T>& rhs) const
	{
		_Point<T> copy = *this;
		return copy += rhs;
	}
	inline _Point<T> operator-(const _Point<T>& rhs) const
	{
		_Point<T> copy = *this;
		return copy -= rhs;
	}
	inline _Point<T> operator-() const
	{
		return _Point<T>(*this) * (T)-1.0;
	}
	//vector
	inline _Point<T>& operator*=(const T& rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		return *this;
	}
	inline _Point<T> operator*(const T& rhs) const
	{
		_Point<T> res = *this;
		return (res *= rhs);
	}
	inline _Point<T>& operator/=(const T& rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		return *this;
	}
	inline _Point<T> operator/(const T& rhs) const
	{
		_Point<T> res = *this;
		return (res /= rhs);
	}
	inline T operator*(const _Point<T>& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	//operators concerning the length
	inline bool operator>(const _Point<T>& rhs)
	{
		return lengthSq() > rhs.lengthSq();
	}
	inline bool operator<(const _Point<T>& rhs)
	{
		return lengthSq() < rhs.lengthSq();
	}
	inline bool operator>=(const _Point<T>& rhs)
	{
		return lengthSq() >= rhs.lengthSq();
	}
	inline bool operator<=(const _Point<T>& rhs)
	{
		return lengthSq() <= rhs.lengthSq();
	}

	//helper functions
	inline T lengthSq() const
	{
		return x * x + y * y;
	}
	inline T length() const
	{
		return sqrt(lengthSq());
	}
	inline _Point<T> normalize() const
	{
		_Point<T> res(*this);
		T len = length();
		if (len != (T)0)
			return res /= len;
		else
			return res;
	}
	inline bool isClose(const _Point<T>& pt, const float maxDistance) const
	{
		return ((maxDistance * maxDistance) > (((*this) - pt).lengthSq()));
	}
	inline T cross(const _Point<T>& rhs) const
	{
		return x * rhs.y - y * rhs.x;
	}
	inline _Point<T> ZeroX() const
	{
		return _Point < T > {(T)0.0, y};
	}
	inline _Point<T> ZeroY() const
	{
		return _Point < T > {x, (T)0.0};
	}
	inline bool isParallel(const _Point<T>& other) const
	{
		T n;
		if (x != 0)
		{
			n = other.x / x;
		}
		else if (y != 0)
		{
			n = other.y / y;
		}
		else
		{
			n = (T)1.0;
		}
		return ((*this) * n == other);
	}
	enum side
	{
		left,
		right
	};
	inline side whichSideVec(const _Point<T>& vec) 
	{
		if (normalize().cross(vec.normalize()) > (T)0.0)
		{
			return left;
		}
		else
		{
			return right;
		}
	}
	//Rotation
	inline _Point<T> CW90() const
	{
		return _Point < T > {y, -x};
	}
	inline _Point<T> CCW90() const
	{
		return _Point< T > {-y, x};
	}
	inline _Point<T> CW45() const
	{
		return (*this + (*this).CW90()) / 2;
	}
	inline _Point<T> CCW45() const
	{
		return (*this + (*this).CCW90()) / 2;
	}
	inline _Point<T> rot(float angle) const
	{
		_Point<T> res = *this;
		float cost = cosf(angle);
		float sint = sinf(angle);
		res.x = x * cost - y * sint;
		res.y = y * cost + x * sint;
		return res;
	}
};

using PointF = _Point< float >;
using PointI = _Point< int >;

#define _Vector _Point //more explicit
using VectorF = PointF;
using VectorI = PointI;