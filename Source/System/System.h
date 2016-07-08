#pragma once
#include "../SDL/SDL.h"
#include <string>
#include "../Utility/SDL_Exception.h"
#include "Log.h"
#include <time.h>

class System
{
public:
	enum BoxIcon
	{
		IconWarning = SDL_MESSAGEBOX_WARNING,
		IconInfo = SDL_MESSAGEBOX_INFORMATION,
		IconError = SDL_MESSAGEBOX_ERROR
	};
public:
	// initialized net an sdl library
	static void Init()
	{
		Uint32 flags = SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_EVENTS;
		if (SDL_Init(flags) != 0)
			throw SDL_Exception("sdl init failed");

		if (SDLNet_Init() != 0)
			throw SDL_NetException("sdl net init failed");

		// seed random
		srand((unsigned int)time(nullptr));
	}
	static void Shutdown()
	{
		SDLNet_Quit();
		SDL_Quit();
	}
	static void EnableScreenSaver()
	{
		SDL_EnableScreenSaver();
	}
	static void DisableScreenSaver()
	{
		SDL_DisableScreenSaver();
	}
	static bool ScreenSaverEnabled()
	{
		return SDL_IsScreenSaverEnabled() != SDL_FALSE;
	}
	static void MessageBox(const std::string& title, const std::string& message, BoxIcon icon)
	{
		SDL_MessageBoxData dat;
		dat.flags = icon;
		dat.window = nullptr;
		dat.title = title.c_str();
		dat.message = message.c_str();

		// buttons
		SDL_MessageBoxButtonData btn;
		btn.buttonid = 0;
		btn.flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
		btn.text = "ok";

		dat.numbuttons = 1;
		dat.buttons = &btn;

		dat.colorScheme = nullptr;

		int pressed;
		SDL_ShowMessageBox(&dat, &pressed);
	}
	static void Sleep(Uint32 ms)
	{
		SDL_Delay(ms);
	}

	// Clipboard
	static bool HasClipboardText()
	{
		return SDL_HasClipboardText() != SDL_FALSE;
	}
	static std::string GetClipboardText()
	{
		char* txt = SDL_GetClipboardText();
		std::string res;
		if (txt)
		{
			res = txt;
			SDL_free(txt);
		}
		return res;
	}
	static void SetClipboardText(const std::string& t)
	{
		if (!SDL_SetClipboardText(t.c_str()))
		{
			Log::Warning("could not set clipboard text");
			Log::Warning(SDL_Exception("SetClipboardText").what());
		}
	}

	// screen keyboard
	static bool HasScreenKeyboard()
	{
		return SDL_HasScreenKeyboardSupport() != SDL_FALSE;
	}
	static void StartTextInput()
	{
		SDL_StartTextInput();
	}
	static void StopTextInput()
	{
		SDL_StopTextInput();
	}
private:
	System(){}
};
