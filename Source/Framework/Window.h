#pragma once
#include "../SDL/SDL.h"
#include <string>
#include "../Utility/Point.h"
#include "../Utility/SDL_Exception.h"
#include "Graphic.h"
#include <memory>
#include "../Utility/Timer.h"
#include "../Utility/Thread.h"
#include "Framework.h"
#include "../Utility/Semaphore.h"
#include "../Utility/Mutex.h"
#include "../Utility/LockGuard.h"
#include <queue>
#include "../System/System.h"
#include "Sound.h"
#include "GameState.h"

class Window : public Thread
{
public:
	Window()
	{}
	void Init(const std::string& name, PointI metrics);

	bool InitGraphics();


	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window()
	{
		Close();
	}
	void Show()
	{
		if (pWnd)
		{
			SDL_ShowWindow(pWnd);
		}
	}
	void Hide()
	{
		if (pWnd)
		{
			SDL_HideWindow(pWnd);
		}
	}
	void Close()
	{
		if (bClosed)
			return;

		bRunning = false;

		Database::Shutdown();
		Sound::Shutdown();
		Thread::Join();

		pGfx.reset();

		if (pWnd)
		{
			SDL_DestroyWindow(pWnd);
			pWnd = nullptr;
		}

		bClosed = true;
	}
	static void SetFullscreen(bool full);
	static void DisplayAchievement(Settings::Acvs a);
	void Run();

private:
	virtual int ThreadProc() override;

	void SwapBuffers()
	{
		SDL_GL_SwapWindow(pWnd);
	}
	void DrawLoadingScreen();
	void LoadIcon();
	void DrawAchievementInfo(Settings::Acvs a, float percent);
	void HandleEvents();
	void UpdateState(Timer& t);
private:
	SDL_Window* pWnd = nullptr;
	SDL_GLContext glContext = nullptr;
	std::unique_ptr< Graphic > pGfx;
	bool bRunning = true;
	bool bClosed = false;
	float lastCodeTime = 0.0f;

	PointI clientArea;
	std::string wndName;

	Semaphore sema;
	Mutex muGfx;

	Mutex muAcv;
	std::queue< Settings::Acvs >  acvqueue;

	std::unique_ptr< GameState > curState;

	int FingersDown = 0;
	bool bMoveFinger = false;
	bool bGfxInit = false;
};
