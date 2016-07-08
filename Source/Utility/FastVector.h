#pragma once
#include <memory>
#include <utility>
#include <exception>
#include <assert.h>
#include <stdexcept>
#include <initializer_list>

// define this to determine the reallocation intervall
// of FastVector
#ifndef FVEC_REALLOC
#define FVEC_REALLOC 1600
#elif FVEC_REALLOC <= 0
#error FVEC_REALLOC must be greater than 0!
#endif

// set this to 1 if you want to enable the copy constructor
#ifndef FVEC_COPYABLE
#define FVEC_COPYABLE 0
#endif

// set this to 0 if you want to disable the move constructor
#ifndef FVEC_MOVEABLE
#define FVEC_MOVEABLE 1
#endif

// set this to 0 if you want to call the copy constructor when pushing elements
// if this is set to 1 the element will be copied via memcpy
#ifndef FVEC_PUSHMEMCPY
#define FVEC_PUSHMEMCPY 0
#endif

template <class T>
class FastVector
{
public:
	FastVector()
	{}
	FastVector(size_t size)
	{
		reserve(size);
	}
	FastVector(const std::initializer_list<T> list)
		:
		lenUsed(list.size())
	{
		// sets lenAlloc and pData
		reserve(lenUsed);
		memcpy(pData, list.begin(), lenAlloc);
	}
	virtual ~FastVector()
	{
		Dispose();
	}
#if FVEC_MOVEABLE != 0
	FastVector(FastVector&& move)
	{
		Swap(*this, move);
	}
	FastVector& operator=(FastVector&& move)
	{
		Swap(*this, move);
		return *this;
	}
#endif

#if FVEC_COPYABLE != 0
	FastVector(const FastVector& copy)
		:
		lenUsed(copy.lenUsed),
		lenAlloc(copy.lenAlloc)
	{
		if (lenAlloc)
		{
			pData = (T*)malloc(lenAlloc);
			if (!pData)
				throw std::bad_alloc();
			memcpy(pData, copy.pData, lenAlloc);
		}
		else
		{
			pData = nullptr;
		}
	}
	FastVector& operator=(FastVector copy)
	{
		Swap(*this, copy);
		return *this;
	}
#else
	FastVector(const FastVector& copy) = delete;
	FastVector& operator=(const FastVector& copy) = delete;
#endif
	void push_back(const T& t)
	{
		lenUsed++;
		if (lenUsed * sizeof(T) > lenAlloc)
			Realloc();
#if FVEC_PUSHMEMCPY != 0
		memcpy(pData + lenUsed - 1, &t, sizeof(T));
#else
		*(pData + lenUsed - 1) = t;
#endif
	}
	void pop_back()
	{
		assert(lenUsed > 0);
		lenUsed--;
	}
	// doesnt check if element is within bounds
	inline T& operator[](size_t pos)
	{
		assert(pos >= lenUsed);
		return *(pData + pos);
	}
	inline const T& operator[](size_t pos) const
	{
		assert(pos >= lenUsed);
		return *(pData + pos);
	}
	// checks if element is within bounds
	T& at(size_t pos)
	{
		if (pos >= lenUsed)
			throw std::out_of_range("FastVector");
		return *(pData + pos);
	}
	const T& at(size_t pos) const
	{
		if (pos >= lenUsed)
			throw std::out_of_range("FastVector");
		return *(pData + pos);
	}
	void clear()
	{
		lenUsed = 0;
	}
	size_t length() const
	{
		return lenUsed;
	}
	size_t capacity() const
	{
		return lenAlloc / sizeof(T);
	}
	bool empty() const
	{
		return (lenUsed == 0);
	}
	T* begin()
	{
		return pData;
	}
	T* end()
	{
		return pData + lenUsed;
	}
	const T* begin() const
	{
		return pData;
	}
	const T* end() const
	{
		return pData + lenUsed;
	}
	// reserves space for size elements
	void reserve(size_t size)
	{
		size *= sizeof(T);
		if (size > lenAlloc)
		{
			lenAlloc = size;
			pData = (T*)realloc(pData, lenAlloc);
			if (!pData)
				throw std::bad_alloc();
		}
	}
	// assigns num elements with val (0 - num-1)
	void assign(size_t num, const T& val)
	{
		reserve(num);
		for (T* i = pData, *end = pData + num; i != end; i++)
			*i = val;
	}
private:
	void Dispose()
	{
		free(pData);
		pData = nullptr;
	}
	void Realloc()
	{
		lenAlloc += FVEC_REALLOC;
		pData = (T*)realloc(pData, lenAlloc);
		if (!pData)
			throw std::bad_alloc();
	}
protected:
	static void Swap(FastVector& l, FastVector& r)
	{
		std::swap(l.lenAlloc, r.lenAlloc);
		std::swap(l.pData, r.pData);
		std::swap(l.lenUsed, r.lenUsed);
	}
private:
	size_t lenAlloc = 0;
	size_t lenUsed = 0;
	T* pData = nullptr;
};