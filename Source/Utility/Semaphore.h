#pragma once
#include "../SDL/SDL.h"
#include "../SDL/SDL_thread.h"
#include "../SDL/SDL_mutex.h"
#include "SDL_Exception.h"

class Semaphore
{
public:
	Semaphore()
	{
		pSem = SDL_CreateSemaphore(1);
		if (!pSem)
			throw SDL_Exception("semaphore creation failed");
	}
	~Semaphore()
	{
		if (pSem)
		{
			SDL_DestroySemaphore(pSem);
			pSem = nullptr;
		}
	}
	Semaphore(const Semaphore&) = delete;
	Semaphore& operator=(const Semaphore&) = delete;

	void Post()
	{
		if (SDL_SemPost(pSem) != 0)
			throw SDL_Exception("SemPost");
	}
	void Wait() const
	{
		if (SDL_SemWait(pSem) != 0)
			throw SDL_Exception("SemWait");
	}
	bool WaitTimeout(Uint32 ms) const
	{
		switch (SDL_SemWaitTimeout(pSem, ms))
		{
		case 0:
			return true;
		case SDL_MUTEX_TIMEDOUT:
			return false;
		default:
			throw SDL_Exception("SemWaitTimeout");
		}
	}
	bool TryWait() const
	{
		switch (SDL_SemTryWait(pSem))
		{
		case 0:
			return true;
		case SDL_MUTEX_TIMEDOUT:
			return false;
		default:
			throw SDL_Exception("SemTryWait");
		}
	}
	Uint32 GetValue() const
	{
		return SDL_SemValue(pSem);
	}
private:
	SDL_sem* pSem = nullptr;
};
