#include "Input.h"
#include "GameState.h"
#include "Framework.h"

static GameState* curRecv = nullptr; // current imput reciever

static PointF mpos;

void Input::Init()
{
	
}
PointF Input::GetMouseXY()
{
	return mpos;
}
void Input::SetReceiver(class GameState* recv)
{
	curRecv = recv;
}
void Input::GameKeyDown(const GameKey& k)
{
	if (curRecv)
	{
		curRecv->Event_GameKeyDown(k, mpos);
		for (auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_GameKeyDown(k, mpos);
		}
	}
}
void Input::GameKeyUp(const GameKey& k)
{
	if (curRecv)
	{
		curRecv->Event_GameKeyUp(k, mpos);
		for (auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_GameKeyUp(k, mpos);
		}
	}
}
void Input::KeyDown(SDL_Scancode s)
{
	if (curRecv)
	{
		curRecv->Event_KeyDown(s);
		for (const auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_KeyDown(s);
		}

		const GameKey gk = Settings::KeyToGamekey(s);
		if (gk != GK_NONE)
		{
			GameKeyDown(gk);
		}
	}
}
void Input::KeyUp(SDL_Scancode s)
{
	if (curRecv)
	{
		curRecv->Event_KeyUp(s);
		for (const auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_KeyUp(s);
		}

		const GameKey gk = Settings::KeyToGamekey(s);
		if (gk != GK_NONE)
		{
			GameKeyUp(gk);
		}
	}
}
void Input::CharDown(char c)
{
	if (curRecv)
	{
		curRecv->Event_Char(c);
		for (const auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_Char(c);
		}
	}
}
void Input::SetMouseNorm(PointF pos)
{
	mpos = Framework::Denormalize(pos);
	if (curRecv)
	{
		curRecv->Event_MouseMove(mpos);
		for (const auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_MouseMove(mpos);
		}
	}
}
void Input::SetMouse(PointI pos)
{
	mpos = Framework::ConvertClientPoint(pos);
	if (curRecv)
	{
		curRecv->Event_MouseMove(mpos);
		for (const auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_MouseMove(mpos);
		}
	}
}
void Input::MouseDown(Uint8 button)
{
	if (curRecv)
	{
		curRecv->Event_MouseDown((MouseKey)button, mpos);
		for (const auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_MouseDown((MouseKey)button, mpos);
		}

		const GameKey gk = Settings::MouseToGamekey((MouseKey)button);
		if (gk != GK_NONE)
			GameKeyDown(gk);
	}
}
void Input::MouseUp(Uint8 button)
{
	if (curRecv)
	{
		curRecv->Event_MouseUp((MouseKey)button, mpos);
		for (const auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_MouseUp((MouseKey)button, mpos);
		}

		const GameKey gk = Settings::MouseToGamekey((MouseKey)button);
		if (gk != GK_NONE)
			GameKeyUp(gk);
	}
}
void Input::Wheel(float amount)
{
	if (curRecv)
	{
		curRecv->Event_MouseWheel(amount, mpos);
		for (const auto& o : curRecv->inRec)
		{
			if (o->isEnabled())
				o->Event_MouseWheel(amount, mpos);
		}

		MouseKey k = amount > 0 ? MouseKey::ScrollDown : MouseKey::ScrollUp;
		const GameKey gk = Settings::MouseToGamekey(k);
		if (gk != GK_NONE)
			GameKeyDown(gk);
	}
}

std::string Input::KeyToAscii(int code)
{
	return std::string(SDL_GetScancodeName((SDL_Scancode)code));
}