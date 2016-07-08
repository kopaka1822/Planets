#pragma once
#include "../SDL/SDL.h"
#include "../SDL/SDL_thread.h"
#include "../SDL/SDL_mutex.h"
#include "SDL_Exception.h"

class Mutex
{
public:
	Mutex()
	{
		pMutex = SDL_CreateMutex();
		if (!pMutex)
			throw SDL_Exception("mutex creation failed");
	}
	Mutex(const Mutex&) = delete;
	Mutex& operator=(const Mutex&) = delete;
	~Mutex()
	{
		SDL_DestroyMutex(pMutex);
	}
	void Lock() const
	{
		if (SDL_LockMutex(pMutex) != 0)
			throw SDL_Exception("mutex lock failed");
	}
	void Unlock() const
	{
		if (SDL_UnlockMutex(pMutex) != 0)
			throw SDL_Exception("mutex unlock failed");
	}

	// returns true if lock succeded
	bool TryLock() const
	{
		int res = SDL_TryLockMutex(pMutex);
		if (res == 0)
			return true;
		else if (res != SDL_MUTEX_TIMEDOUT)
			throw SDL_Exception("mutex try lock failed");

		return false;
	}
private:
	SDL_mutex* pMutex;
};
