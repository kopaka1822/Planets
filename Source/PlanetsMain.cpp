#include "System/Log.h"
#include "System/System.h"
#include "Framework/Input.h"
#include "Framework/Window.h"
#include "Framework/Settings.h"

int main(int argc, char** argv)
{
	Log::NewLog();
	try
	{
		Log::Write("initializing SDL");
		System::Init();
		LOGDEBUG("initializing settings");
		Settings::Initialize();
		LOGDEBUG("initializing input");
		Input::Init();

		PointI screen = { 1280, 720 };
		// screen to small?
		if(SDL_Init(SDL_INIT_VIDEO))
			throw SDL_Exception("could not init sdl video");

		for (int i = 0; i < SDL_GetNumVideoDisplays(); i++)
		{
			SDL_DisplayMode mode;
			if (SDL_GetCurrentDisplayMode(i, &mode) == 0)
			{
				if (screen.x > mode.w || screen.y > mode.h)
					screen = { 800, 600 };
			}
		}


		Window wnd;

		wnd.Init("Planets BETA", screen);

		wnd.Run();

		wnd.Close();
	}
	catch (const std::exception& e)
	{
		Log::Error(e);
	}

	Settings::Shutdown();
	System::Shutdown();
	Log::Shutdown();
	return 0;
}
