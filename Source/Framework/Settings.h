#pragma once
#include <string>
#include "../Utility/Point.h"
#include <vector>
#include "Input.h"

using byte = unsigned char;

class Settings
{
public:
	struct Achievement
	{
		std::string title;
		std::string desc;
		bool unlocked;
	};
	enum Acvs
	{
		A_MenuEaster,
		A_9000,
		A_MapBuilder,
		A_Cookie,
		A_Online,
		A_KillMenu,
		A_ReadyButton,
		A_LoseGame,
		A_WinGame,
		A_GG,
		A_Music,
		A_AllLevel,
		A_FirstPlanet,
		A_FirstStart,
		A_DJ,
		A_MLG,
		A_Click1000000,
		A_SIZE
	};
	struct ServerInfo
	{
		std::string nick;
		std::string IP;
		short port;
	};
public:
	static void Initialize();
	static void Shutdown();

	//Properties
	static void Save();
	static void RestoreLast();
	static void RestoreDefault();

	static float GetVolMusic();
	static float GetVolSound();
	static void SetVolMusic(float);
	static void SetVolSound(float);

	static std::string GetName();
	static std::string GetClan();
	static void SetName(std::string);
	static void SetClan(std::string);

	static bool GetFullscreen();
	static void SetFullscreen(bool);

	static unsigned long long GetMenuScore();
	static void SetMenuStore(unsigned long long val);

	static Input::GKstruct GetGamekey(Input::GameKey g);
	static void SetGamekey(Input::GameKey g, Input::GKstruct s);

	static Input::GameKey MouseToGamekey(Input::MouseKey k);
	static Input::GameKey KeyToGamekey(int code);

	static std::vector<ServerInfo> GetServerInfos();
	static void SetServerInfos(std::vector<ServerInfo>&& inf);

	static const std::vector< Achievement >& GetAchievements();
	static bool UnlockAchievement(Acvs a);

	static class Color GetColor(byte team);
	static void SetColor(byte team, class Color c);
	static void LoadDefaultColors();

	// Game Controls
	static bool DeselectOnTargetSelect();
	static void SetDeselectOnTargetSelect(bool b);

	// Game graphics
	static void SetDetailLevel(float l);
	static float GetDetailLevel();
	static bool PlanetGlowEnabled();
	static bool TargetLinesEnabled();
	static void SetTargetLines(bool b);

	static float GetPlanetGlowFactor();
	static void SetPlanetGlowFactor(float f);

	static void SetPerformanceInfo(bool b);
	static bool GetPerformanceInfo();

	static unsigned int GetShader();
	static void SetShader(unsigned int s);

	// FXAA
	static float GetFXAASpan();
	static float GetFXAAMul();
	static float GetFXAAShift();
	static void SetFXAASpan(float f);
	static void SetFXAAMul(float f);
	static void SetFXAAShift(float f);
private:
	static void LoadFromFile();
	static void Apply();
};