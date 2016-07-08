#pragma once
#include "Mutex.h"

class LockGuard
{
public:
	LockGuard(const Mutex& mu)
		:
		mtx(mu)
	{
		mtx.Lock();
	}
	LockGuard(const LockGuard&) = delete;
	LockGuard& operator=(const LockGuard&) = delete;
	void Unlock()
	{
		if (bLocked)
		{
			mtx.Unlock();
			bLocked = false;
		}
	}
	~LockGuard()
	{
		if (bLocked)
		{
			mtx.Unlock();
			bLocked = false;
		}
	}
private:
	const Mutex& mtx;
	bool bLocked = true;
};