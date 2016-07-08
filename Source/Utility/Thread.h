#pragma once
#include "../SDL/SDL.h"
#include "../SDL/SDL_thread.h"
#include <exception>
#include "SDL_Exception.h"
#include <assert.h>
#include "../System/System.h"

class Thread
{
protected:
	enum Priority
	{
		LOW = SDL_THREAD_PRIORITY_LOW,
		NORMAL = SDL_THREAD_PRIORITY_NORMAL,
		HIGH = SDL_THREAD_PRIORITY_HIGH
	};
public:
	Thread()
	{}
	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	virtual ~Thread()
	{}
	void Begin()
	{
		assert(!bRunning);
		if (!bRunning)
		{
			pThread = SDL_CreateThread(ThreadCallback, "TestThread", (void*)this);
			if (pThread == nullptr)
				throw SDL_Exception("thread creation failed");
			bRunning = true;
		}
	}
	void Detach()
	{
		if (pThread)
		{
			SDL_DetachThread(pThread);
			pThread = nullptr;
		}
	}
	void Join()
	{
		if (pThread)
		{
			SDL_WaitThread(pThread, &retVal);
			pThread = nullptr;
		}
	}
	bool HasThreadError() const
	{
		return threadError.length() != 0;
	}
	std::string GetThreadError()
	{
		return threadError;
	}
	bool IsRunning() const
	{
		return bRunning;
	}
protected:
	// Sets priority of the calling thread
	void SetThreadPriority(Priority p)
	{
		if (SDL_SetThreadPriority((SDL_ThreadPriority)p) != 0)
			throw SDL_Exception("could not set thread prioritiy");
	}
	virtual int ThreadProc() = 0;
	void SetThreadError(const std::string& m)
	{
		threadError = m;
	}
private:
	static int ThreadCallback(void* pThread)
	{
#if _DEBUG
		try
		{
#endif
			auto ret = ((Thread*)pThread)->ThreadProc();
			((Thread*)pThread)->bRunning = false;
			return ret;
#if _DEBUG
		}
		catch (const std::exception& e)
		{
			System::MessageBox("ERROR", e.what(), System::IconError);
		}
		return -1;
#endif
	}
private:
	SDL_Thread* pThread = nullptr;
	int retVal;
	std::string threadError;
	bool bRunning = false;
};
