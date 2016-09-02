#pragma once
#include "../SDL/SDL.h"
#include "../Utility/Point.h"
#include <string>

class Input
{
	friend class Window;

public:
	enum GameKey
	{
		GK_NONE,
		GK_SELECT,
		GK_CLICK,
		GK_DESELECT,
		GK_SELECTALL,
		GK_MAKEGROUP,
		GK_ADDGROUP,
		GK_SCROLL,
		GK_ZOOMIN,
		GK_ZOOMOUT,
		GK_CHAT,
		GK_PLAYERLIST,
		GK_FILTERENT,
		GK_FILTERBOMB,
		GK_FILTERSPEED,
		GK_FILTERSABO,
		GK_PLANDEFENSE, // all planets on defense
		GK_TURBO,

		GK_SIZE
	};
	struct GKstruct
	{
		enum type
		{
			Key,
			Mouse,
			None
		} t;
		int code;
		bool operator==(const GKstruct& o) const
		{
			return (code == o.code) && (t == o.t);
		}
		bool operator!=(const GKstruct& o) const
		{
			return !(*this == o);
		}
	};
	enum MouseKey
	{
		Left = SDL_BUTTON_LEFT,
		Right = SDL_BUTTON_RIGHT,
		Middle = SDL_BUTTON_MIDDLE,
		X1 = SDL_BUTTON_X1,
		X2 = SDL_BUTTON_X2,
		ScrollUp,
		ScrollDown
	};
	class EventReceiver
	{
		friend Input;
	public:
		virtual void Event_KeyDown(SDL_Scancode code){}
		virtual void Event_KeyUp(SDL_Scancode code){}
		virtual void Event_GameKeyDown(GameKey k,const PointF& p){}
		virtual void Event_GameKeyUp(GameKey k,const PointF& p){}
		virtual void Event_Char(char c){}
		virtual void Event_MouseDown(MouseKey k,const PointF& pos){}
		virtual void Event_MouseUp(MouseKey k,const PointF& pos){}
		virtual void Event_MouseMove(const PointF& pos){}

		//positive = forwards | negative = backwards
		virtual void Event_MouseWheel(float amount,const PointF& pos){}
	};
public:
	static void Init();

	static PointF GetMouseXY();
	static void SetReceiver(class GameState* recv);
	static std::string KeyToAscii(int code);
private:
	static void KeyDown(SDL_Scancode s);
	static void KeyUp(SDL_Scancode s);
	static void CharDown(char c);

	static void SetMouse(PointI pos);
	static void SetMouseNorm(PointF pos); // normalized coordineates 0..1
	static void MouseDown(Uint8 button);
	static void MouseUp(Uint8 button);
	
	static void Wheel(float amount);

	static void GameKeyDown(const GameKey& k);
	static void GameKeyUp(const GameKey& k);
};