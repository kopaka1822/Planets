#include "Window.h"
#include "../Utility/GameError.h"

// STATES
#include "MenuState.h"
#include "StateExtras.h"
#include "StateOptions.h"
#include "StateAchiev.h"
#include "StateError.h"
#include "StateSingle.h"
#include "StateGameSingle.h"
#include "StateGameOver.h"
#include "StatePause.h"
#include "StateEditor.h"
#include "StateEditorTeams.h"
#include "StateServerSelect.h"
#include "StateMultiName.h"
#include "StateConnect.h"
#include "StateMulLobby.h"
#include "StateMulLobbyDyn.h"
#include "StateMulLobbyFixed.h"
#include "StateGameMulti.h"
#include "ServerStartPage.h"
#include "StateColor.h"
#include "StateInit.h"
#include "StateLevelpackEditor.h"
#include "StateLevelpackWin.h"
#include "StateFirstStart.h"
#include "StateGameOverGraph.h"
#include "StateSelectTutorial.h"
#include "StateGameTutorial.h"
#include "StateGameTutorial2.h"
#include "StateTutorialProposal.h"

#include "../Utility/Exception.h"

Window* MainWindow = nullptr;

void Window::Init(const std::string& name, PointI metrics)
{
	Log::Write("initializing window");
	MainWindow = this;

	clientArea = metrics;
	wndName = name;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// multisampling
	/*SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);*/
	Log::Write("creating window");

	pWnd = SDL_CreateWindow(wndName.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		clientArea.x, clientArea.y,
		SDL_WindowFlags::SDL_WINDOW_OPENGL
		| SDL_WindowFlags::SDL_WINDOW_RESIZABLE
		);

	if (!pWnd)
		throw SDL_Exception("window creation failed");

	LoadIcon();
	SDL_SetWindowMinimumSize(pWnd, 800, 600);

	if (Settings::GetFullscreen())
		SDL_SetWindowFullscreen(pWnd, SDL_WINDOW_FULLSCREEN_DESKTOP);

	SDL_GetWindowSize(pWnd, &clientArea.x, &clientArea.y);

	// Initialize Graphics on the render thread
	// Lock semaphore for graphics init
	sema.Wait();
	Log::Write("starting graphics thread");
	Thread::Begin();

	DrawLoadingScreen();
	// wait unitl graphics completes
	sema.WaitTimeout(20000);

	if (Thread::HasThreadError() || !bGfxInit)
	{
		throw Exception(GetThreadError());
	}
	Log::Write("graphics thread initialized");
}
void Window::LoadIcon()
{
	SDL_Surface* pIcon = SDL_LoadBMP("data/pic/app.bmp");
	if (pIcon)
	{
		SDL_SetWindowIcon(pWnd, pIcon);

		SDL_FreeSurface(pIcon);
	}
	else
	{
		Log::Warning("could not load icon");
		Log::Warning(SDL_Exception("SDL_LoadBMP(data/pic/app.ico)").what());
	}
}
void Window::DrawLoadingScreen()
{
	SDL_Renderer* pScreen = nullptr;
	SDL_Surface* pImage = nullptr;
	SDL_Texture* bgtx = nullptr;

	try
	{
		Log::Write("drawing fancy loading screen");
		Log::Write("creating renderer");
		pScreen = SDL_CreateRenderer(pWnd, -1, SDL_RENDERER_ACCELERATED);
		if (!pScreen)
			throw SDL_Exception("SDL_CreateRenderer");

		Log::Write("loading image");
		pImage = SDL_LoadBMP("data/pic/load.bmp");
		if (!pImage)
			throw SDL_Exception("SDL_LoadBMP(data/pic/load.bmp)");

		bgtx = SDL_CreateTextureFromSurface(pScreen, pImage);
		if (!bgtx)
			throw SDL_Exception("SDL_CreateTextureFromSurface");

		// calculating rectangle
		SDL_Rect rect;
		rect.w = pImage->w;
		rect.h = pImage->h;

		SDL_FreeSurface(pImage);
		pImage = nullptr;

		rect.x = (clientArea.x - rect.w) / 2;
		rect.y = (clientArea.y - rect.h) / 2;

		SDL_SetRenderDrawColor(pScreen, 255, 255, 255, 255);
		SDL_RenderClear(pScreen);
		SDL_RenderCopy(pScreen, bgtx, NULL, &rect);
		SDL_RenderPresent(pScreen);
	}
	catch (const std::exception& e)
	{
		Log::Warning("could not draw loading screen");
		Log::Warning(e.what());
	}

	if (pImage)
		SDL_FreeSurface(pImage);

	if (bgtx)
		SDL_DestroyTexture(bgtx);

	if (pScreen)
		SDL_DestroyRenderer(pScreen);
}

bool Window::InitGraphics()
{
	try
	{
		// Open gl HAS to be created on render thread

		Log::Write("creating openGL context");

		glContext = SDL_GL_CreateContext(pWnd);
		if (!glContext)
			throw SDL_Exception("open gl context could not be created");

		Log::Write("initializing openGL");

		pGfx = std::unique_ptr< Graphic >(new Graphic());

		Log::Write("sending openGL data to GPU");
		pGfx->Create(glContext, clientArea.x, clientArea.y);

		Log::Write("setting swap intervall");
		SDL_GL_SetSwapInterval(1); // vsynch

		Log::Write("initializing database");
		Database::Initialize(pGfx->draw);

		// ready
		bGfxInit = true;
		sema.Post();
	}
	catch (const std::exception& e)
	{
		//Log::Error(e);
		SetThreadError(e.what());
		return false;
	}
	catch (...)
	{
		//Log::Error("unkown graphics error");
		SetThreadError("unkown graphics error");
		return false;
	}


	return true;
}

void Window::Run()
{
	// initialize
	//curState = std::unique_ptr< GameState >(new MenuState());
	curState = std::unique_ptr<StateInit>(new StateInit());
	Input::SetReceiver(curState.get());

	Timer t;
	t.StartWatch();

	while (bRunning)
	{
		HandleEvents();
		UpdateState(t);
	}

	bRunning = false;
	Thread::Join();
}
void Window::HandleEvents()
{
	SDL_Event msg;
	while (SDL_PollEvent(&msg))
	{
		switch (msg.type)
		{
		case SDL_EventType::SDL_QUIT:
			bRunning = false;
			{
				LockGuard g(muGfx);
				curState.reset();
			}
			break;

			/////////////////////////////////////
			/////////////// Mouse ///////////////
			/////////////////////////////////////

		case SDL_EventType::SDL_MOUSEMOTION:
			if (FingersDown < 2)
				Input::SetMouse(PointI(msg.motion.x, msg.motion.y));
			break;
		case SDL_EventType::SDL_MOUSEBUTTONDOWN:
			if (FingersDown < 2)
				Input::MouseDown(msg.button.button);
			break;
		case SDL_EventType::SDL_MOUSEBUTTONUP:
			if (FingersDown < 2)
				Input::MouseUp(msg.button.button);
			break;
		case SDL_EventType::SDL_MOUSEWHEEL:
			//if (!FingersDown)
				Input::Wheel((float)msg.wheel.y);
			break;

			/////////////////////////////////////
			/////////////// Touch ///////////////
			/////////////////////////////////////

		case SDL_EventType::SDL_FINGERDOWN:
			FingersDown++;
			break;
		case SDL_EventType::SDL_FINGERUP:
			FingersDown--;
			break;
		case SDL_EventType::SDL_MULTIGESTURE:
			if (msg.mgesture.numFingers >= 2)
			{
				// movement
				if (!bMoveFinger)
				{
					Input::SetMouseNorm({ msg.mgesture.x, msg.mgesture.y });
					Input::MouseDown(Input::MouseKey::Middle);
					bMoveFinger = true;
				}
				Input::SetMouseNorm({ msg.mgesture.x, msg.mgesture.y });

				// zoom
				if (msg.mgesture.dDist)
				{
					Input::Wheel(msg.mgesture.dDist * 20.0f);
				}
			}
			break;


			/////////////////////////////////////
			/////////////// Keyboard ////////////
			/////////////////////////////////////

		case SDL_EventType::SDL_KEYUP:
			Input::KeyUp(msg.key.keysym.scancode);
			break;
		case SDL_EventType::SDL_KEYDOWN:
			Input::KeyDown(msg.key.keysym.scancode);
			break;

		case SDL_EventType::SDL_TEXTINPUT:
		{
			const char* c = msg.text.text;
			int len = 0;
			while (*c != '\0' && len < 31)
			{
				Input::CharDown(*c);
				c++;
				len++;
			}
		}
		break;
		/////////////////////////////////////
		/////////////// Graphics ////////////
		/////////////////////////////////////

		case SDL_EventType::SDL_WINDOWEVENT:
			switch (msg.window.event)
			{
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			case SDL_WINDOWEVENT_RESIZED:
				if (pGfx)
				{
					clientArea = PointI(msg.window.data1, msg.window.data2);
					LockGuard guard(muGfx);
					pGfx->Resize(msg.window.data1, msg.window.data2);

					if (curState)
						curState->Event_Resize(Framework::DrawStart(), Framework::DrawWidth());
				}
				break;
				/*
			case SDL_WINDOWEVENT_FOCUS_LOST:
				
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				
				break;
			case SDL_WINDOWEVENT_ENTER:
				
				break;
			case SDL_WINDOWEVENT_LEAVE:
				
				break;*/
			}
			break;
		default:
			break;
		}
	}

	if (FingersDown < 2 && bMoveFinger)
	{

		bMoveFinger = false;
		Input::MouseUp(Input::MouseKey::Middle);

	}
}
void Window::UpdateState(Timer& t)
{
	try
	{
		if (!curState)
			return;

		float dt = t.GetTimeSecond();
		t.StartWatch();

		curState->ExecuteCode(dt);

		lastCodeTime = t.GetTimeMilli();

		// thread friendly
		int tm = (int)t.GetTimeMilli();
		tm = 25 - tm;
		if (tm > 0)
			System::Sleep(tm);

		if (curState->isFinished())
		{
			LockGuard guard(muGfx);

			GameState::states next = curState->GetNextState();
			std::unique_ptr<GameState> old(std::move(curState));

			switch (next)
			{
			case GameState::states::Undefined:
				throw Exception("undefined state");
				break;
			case GameState::states::Exit:
				bRunning = false;
				break;
			case GameState::states::Menu:
				Database::ClearGameStruct(); // just to be sure
				Database::SetMainServer(std::string(), 0);
				curState = std::unique_ptr<MenuState>(new MenuState());
				break;
			case GameState::states::Extras:
				curState = std::unique_ptr<StateExtras>(new StateExtras());
				break;
			case GameState::states::Options:
				curState = std::unique_ptr<GameState>(new StateOptions(std::move(old)));
				break;
			case GameState::states::Achiev:
				curState = std::unique_ptr<StateAchiev>(new StateAchiev());
				break;
			case GameState::states::Single:
				curState = std::unique_ptr<StateSingle>(new StateSingle());
				break;
			case GameState::states::GameSingle:
				curState = std::unique_ptr<StateGameSingle>(new StateGameSingle());
				break;
			case GameState::states::GameOver:
				curState = std::unique_ptr<StateGameOverGraph>(new StateGameOverGraph());
				break;
			case GameState::states::TutorialProposal:
				curState = std::unique_ptr<StateTutorialProposal>(new StateTutorialProposal());
				break;
			case GameState::states::GameOver2:
				curState = std::unique_ptr<StateGameOver2>(new StateGameOver2());
				break;
			case GameState::states::Pause:
				curState = std::unique_ptr<GameState>(new StatePause(std::move(old)));
				break;
			case GameState::states::Editor:
				curState = std::unique_ptr<StateEditor>(new StateEditor());
				break;
			case GameState::states::EditorTeams:
				curState = std::unique_ptr<GameState>(new StateEditorTeams(std::move(old)));
				break;
			case GameState::states::TutoialGame:
				curState = std::unique_ptr<GameState>(new StateGameTutorial());
				break;
			case GameState::states::MultiName:
				curState = std::unique_ptr<StateMultiName>(new StateMultiName());
				break;
			case GameState::states::ServerSelect:
				curState = std::unique_ptr<StateServerSelect>(new StateServerSelect());
				break;
			case GameState::states::Connect:
				curState = std::unique_ptr<StateConnect>(new StateConnect());
				break;
			case GameState::states::StateFirstStart:
				curState = std::unique_ptr<StateFirstStart>(new StateFirstStart());
				break;
			case GameState::states::MultiLobbyDyn:
				curState = std::unique_ptr<StateMulLobbyDyn>(new StateMulLobbyDyn());
				break;
			case GameState::states::TutorialGame2:
				curState = std::unique_ptr<StateGameTutorial2>(new StateGameTutorial2());
				break;
			case GameState::states::MultiLobbyFixed:
				curState = std::unique_ptr<StateMulLobbyFixed>(new StateMulLobbyFixed());
				break;
			case GameState::states::GameMulti:
				curState = std::unique_ptr<StateGameMulti>(new StateGameMulti());
				break;
			case GameState::states::ServerStartPage:
				curState = std::unique_ptr<GameState>(new ServerStartPage(Database::GetGameStruct()));
				break;
			case GameState::states::Color:
				curState = std::unique_ptr<StateColor>(new StateColor());
				break;
			case GameState::states::LevelpackEditor:
				curState = std::unique_ptr<GameState>(new StateLevelPackEditor());
				break;
			case GameState::states::LevelpackWin:
				curState = std::unique_ptr<StateLevelpackWin>(new StateLevelpackWin());
				break;
			case GameState::states::Tutorial:
				curState = std::unique_ptr<GameState>(new StateTutorial());
				break;
			case GameState::states::Back:
				if (old)
					curState = old->GetPrev();

				if (curState)
				{
					curState->Event_Resize(Framework::DrawStart(), Framework::DrawWidth());
					curState->Event_MouseMove(Input::GetMouseXY());
				}
				else
				{
					curState = std::unique_ptr<MenuState>(new MenuState());
				}
				break;
			default:
				throw Exception("illegal state");
			}

			Input::SetReceiver(curState.get());
			PointI rawMouse;
			SDL_GetMouseState(&rawMouse.x, &rawMouse.y);
			Input::SetMouse(rawMouse);
			guard.Unlock();

			t.StartWatch(); // reset dt
		}

		if (Thread::HasThreadError())
		{
			bRunning = false;
		}
	}
	catch (const GameError& e)
	{
		// non critical error (connection timed out etc.)
		LockGuard guard(muGfx);
		curState.reset();
		//Database::ClearGameStruct();
		curState = std::unique_ptr<GameState>(new StateError(e.what()));
		Input::SetReceiver(curState.get());
		guard.Unlock();
	}
	catch (const std::exception& e)
	{
		Log::Error(e.what());
		bRunning = false;
	}
}

void Window::DrawAchievementInfo(Settings::Acvs a, float percent)
{
	static const int border = 6;
	static const int padding = 6;

	if (a < 0 || a >= Settings::A_SIZE)
		return;


	const std::string& desc = Settings::GetAchievements()[a].title;
	const std::string& title = "Achievement Unlocked!";

	const float texlen = std::max(FONT_SMALL.GetMetrics(desc).x, FONT_MED.GetMetrics(title).x);
	const float hei = 2 * border + 3 * padding + FONT_SMALL.GetFontHeight() + FONT_MED.GetFontHeight();
	const float width = 2 * border + 2 * padding + texlen;

	percent = tool::clamp(percent, 0.0f, 1.0f);
	PointF startpos = Framework::DrawStart() - PointF(0.0f, (1.0f - percent) * hei);

	Drawing::GetDraw().DrawBox(RectF(startpos, startpos + PointF(width, hei)),
		(float)border, Color::White(), Color::Black());

	FONT_MED.SetColor(Color::White());
	FONT_SMALL.SetColor(Color::White());

	FONT_MED.Text(title, startpos + PointF(border + padding, border + padding));
	FONT_SMALL.Text(desc, startpos + PointF(border + padding,
		border + 2 * padding + FONT_MED.GetFontHeight()));
}

int Window::ThreadProc()
{
	try
	{
		if (!InitGraphics())
		{
			sema.Post();
			return 0;
		}

		float acvTimer = 0.0f; // Achievement timer
		Settings::Acvs curA = Settings::Acvs::A_SIZE;
		Timer frameTimer; // Achievement

		// FPS and performance
		int nFrames = 0;
		float sumTime = 0.0f;
		int lastFPS = 0;
		Timer performanceTimer;
		Timer drawTimer;

		frameTimer.StartWatch();
		performanceTimer.StartWatch();

		while (bRunning)
		{
			// check for next achievement
			if (acvTimer <= 0.0f)
			{
				LockGuard gacv(muAcv);
				if (!acvqueue.empty())
				{
					curA = acvqueue.front();
					acvqueue.pop();
					acvTimer = 5000.0f; // 5 sec display time
					Sound::Effect(Sound::S_WIN);
				}
			}

			LockGuard guard(muGfx);

			if (!curState.get())
				continue;

			try
			{
				drawTimer.StartWatch();

				pGfx->BeginFrame();

				/////////// State ///////////
				curState->ComposeFrame(pGfx->draw);

				///////// achievement ///////
				float dt = frameTimer.GetTimeMilli();
				frameTimer.StartWatch();

				if (acvTimer > 0.0f && curA != Settings::A_SIZE)
				{
					acvTimer -= dt;
					// draw Achievement
					float percent = acvTimer / 2000.0f;
					if (percent > 1.0f)
						percent = (5000.0f - acvTimer) / 500.0f;

					DrawAchievementInfo(curA, percent);
				}

				/////// FPS ///////////
				drawTimer.StopWatch();
				nFrames++;
				sumTime += performanceTimer.GetTimeMilli();
				performanceTimer.StartWatch();

				if (sumTime >= 1000.0f)
				{
					sumTime -= 1000.0f;
					lastFPS = nFrames;
					nFrames = 0;
				}

				if (Settings::GetPerformanceInfo())
				{
					FONT_SMALL.SetColor(Color::White());
					FONT_SMALL.Text(std::string("FPS: ") + std::to_string(lastFPS), { 10, 10 });
					FONT_SMALL.Text(std::string("code: ") + std::to_string(lastCodeTime), { 10, 50 });
					FONT_SMALL.Text(std::string("draw: ") + std::to_string(drawTimer.GetTimeMilli()), { 10, 90 });
				}
				pGfx->EndFrame();
			}
			catch (const GL_Recreate&)
			{
				guard.Unlock();
				Log::Warning("cought gl recreate");
				pGfx->Dispose();

				LockGuard g2(muGfx);
				// trying to recreate
				Log::Write("trying to recreate gl context");
				auto glContext = SDL_GL_CreateContext(pWnd);
				if (!glContext)
					throw Exception("could not recreate context");
				pGfx->Create(glContext, clientArea.x, clientArea.y);
			}

			guard.Unlock();
			SwapBuffers();

		}

		if (pGfx)
			pGfx->Dispose();
	}
	catch (const std::exception& e)
	{
		Log::Error(e);
		SetThreadError(e.what());
	}
	catch (...)
	{
		Log::Error("undefined draw thread error");
		SetThreadError("unknown");
	}

	return 0;
}

void Window::SetFullscreen(bool full)
{
	if (MainWindow && MainWindow->pWnd)
	{
		if (full)
		{
			SDL_SetWindowFullscreen(MainWindow->pWnd, SDL_WINDOW_FULLSCREEN_DESKTOP);
		}
		else
		{
			SDL_SetWindowFullscreen(MainWindow->pWnd, 0);
		}
	}
}

void Window::DisplayAchievement(Settings::Acvs a)
{
	assert(MainWindow);
	LockGuard guard(MainWindow->muAcv);
	MainWindow->acvqueue.push(a);
	guard.Unlock();
}
