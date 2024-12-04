#include "Settings.h"
#include "../Utility/Exception.h"

// ADD SETTINGS_FULL to preprocessor for game client!

#ifdef SETTINGS_FULL

#include "../System/Log.h"
#include "Sound.h"
#include "Window.h"

// Log acces
#define LOGWRITE(s) Log::Write(s)
#define LOGWARNING(s) Log::Warning(s)
#else
// no log
#define LOGWRITE(s)
#define LOGWARNING(s)
#endif

#include <assert.h>

#include "../Utility/FileReader.h"
#include "../Utility/FileWriter.h"

#include "Color.h"
#include "Input.h"

#include <vector>
#include "../Utility/Tools.h"

static bool bInit = false;

static const char* FILEPATH = "data//config.dat";

//variables
static float volMusic;
static float volSound;
static std::string Name;
static std::string Clan;
static bool bFullscreen;
static Input::GKstruct inp[Input::GameKey::GK_SIZE];
static std::vector<Settings::ServerInfo> servers;
static std::vector<Settings::Achievement> acvments;
Color TeamColors[26];

static float planetGlowFactor;
static bool bDeselectOnTarget;
static bool targetLines;
static float detailLevel;
static bool performanceInfo;
static unsigned int shaderConfig;
static float fxaaSpan;
static float fxaaMul;
static float fxaaShift;
static bool bStarfield;

static const unsigned int curVersion = 25;

static unsigned long long MenuScore;

void LoadAchievementInfo()
{
	LOGWRITE("loading achievements");
	acvments.clear();
	Settings::Achievement a;
	acvments.assign(Settings::A_SIZE, a);
	a.unlocked = false;

	// data
	a.title = "It's over 9000!";
	a.desc = "Reached a score beyond 9000";
	acvments[Settings::A_9000] = a;

	a.title = "Eggs";
	a.desc = "Found a menu easter egg";
	acvments[Settings::A_MenuEaster] = a;

	a.title = "Creator";
	a.desc = "Saved your first map";
	acvments[Settings::A_MapBuilder] = a;

	a.title = "Cookie Clicker";
	a.desc = "Reached a score beyond 100000";
	acvments[Settings::A_Cookie] = a;

	a.title = "Going online";
	a.desc = "Played a game in the Multiplayer Mode";
	acvments[Settings::A_Online] = a;

	a.title = "Take this!";
	a.desc = "Killed an entity in the menu screen";
	acvments[Settings::A_KillMenu] = a;

	a.title = "Ready to rumble";
	a.desc = "Pressed the ready button";
	acvments[Settings::A_ReadyButton] = a;

	a.title = "Nice Guy";
	a.desc = "Let another player win a game";
	acvments[Settings::A_LoseGame] = a;

	a.title = "Ez";
	a.desc = "Won a game";
	acvments[Settings::A_WinGame] = a;

	a.title = "G.G. Anderson";
	a.desc = "Typed gg in the chat";
	acvments[Settings::A_GG] = a;

	a.title = "Boom Boom Boom";
	a.desc = "Set music volume to max";
	acvments[Settings::A_Music] = a;

	a.title = "Planet conqueror";
	a.desc = "Finished all levels";
	acvments[Settings::A_AllLevel] = a;

	a.title = "First Blood";
	a.desc = "Be the first to capture a planet in a multiplayer game";
	acvments[Settings::A_FirstPlanet] = a;

	a.title = "The Beginning";
	a.desc = "Started the game for the first time";
	acvments[Settings::A_FirstStart] = a;

	a.title = "DJ";
	a.desc = "Remixed the Theme Song";
	acvments[Settings::A_DJ] = a;

	a.title = "MLG";
	a.desc = "Captured a planet using a saboteur";
	acvments[Settings::A_MLG] = a;

	a.title = "Autoclicker?";
	a.desc = "Reached a score beyond 1000000";
	acvments[Settings::A_Click1000000] = a;
}
void Settings::LoadDefaultColors()
{
	TeamColors[0] = Color::White();
	TeamColors[1] = Color::Red();
	TeamColors[2] = Color::Blue();
	TeamColors[3] = Color::Green();
	TeamColors[4] = Color::Yellow();
	TeamColors[5] = Color::Zyan();
	TeamColors[6] = Color::Magenta();
	TeamColors[7] = Color(255, 125, 0);
	TeamColors[8] = Color(0, 255, 125);
	TeamColors[9] = Color(125, 0, 255);
	TeamColors[10] = Color(255, 0, 125);
	TeamColors[11] = Color(125, 255, 0);
	TeamColors[12] = Color(0, 125, 255);
	TeamColors[13] = Color(0, 125, 0);
	TeamColors[14] = Color(0, 0, 125);
	TeamColors[15] = Color(125, 0, 0);
	TeamColors[16] = Color(140, 60, 220);
	TeamColors[17] = Color(220, 140, 60);
	TeamColors[18] = Color(60, 220, 140);
	TeamColors[19] = Color(140, 220, 60);
	TeamColors[20] = Color(60, 140, 220);
	TeamColors[21] = Color(220, 60, 140);
	TeamColors[22] = Color(200, 220, 140);
	TeamColors[23] = Color(140, 220, 200);
	TeamColors[24] = Color(200, 140, 220);
	TeamColors[25] = Color(220, 200, 140);
}

void Settings::Initialize()
{
	if (bInit)
		return;

	LoadAchievementInfo();

	LoadFromFile();

	bInit = true;
	LOGWRITE("initialized settings");
}
void Settings::Shutdown()
{
	if (!bInit)
		return;

	//save settings?
	Save();

	bInit = false;
	LOGWRITE("released settings");
}

void Settings::LoadFromFile()
{
	//Load Settings
	FileReader r(FILEPATH);
	if (r.isOpen())
	{
		try
		{
			LOGWRITE("loading settings from file");
			int version = r.readInt();
			if (version < 23 || version > curVersion)
				throw Exception("deprecated config.dat");

			bFullscreen = r.readChar() != 0;
			MenuScore = r.read < unsigned long long >();
			Name = r.readString();
			Clan = r.readString();

			volMusic = r.readFloat();
			volSound = r.readFloat();

			int gkMax = Input::GK_SIZE;
			if (version < 25)
				gkMax = 17;

			for (int i = 0; i < gkMax; i++)
			{
				inp[i].t = Input::GKstruct::type(r.readChar());
				inp[i].code = r.readInt();
			}

			if (version < 25)
				inp[Input::GK_TURBO] = {Input::GKstruct::type::Key,SDL_SCANCODE_S};

			int nServs = r.readInt();
			for (int i = 0; i < nServs; i++)
			{
				ServerInfo info;
				info.nick = r.readString();
				info.IP = r.readString();
				info.port = r.read<short>();
				servers.push_back(std::move(info));
			}

			for (auto& a : acvments)
			{
				a.unlocked = r.readChar() != 0;
			}

			// Colors
			TeamColors[0] = Color::White();
			for (int i = 1; i < 26; i++)
			{
				TeamColors[i].r = r.readFloat();
				TeamColors[i].g = r.readFloat();
				TeamColors[i].b = r.readFloat();
				TeamColors[i].a = 1.0f;
			}

			targetLines = r.readChar() != 0;
			detailLevel = r.readFloat();
			performanceInfo = r.readChar() != 0;
			shaderConfig = r.readInt();

			fxaaSpan = r.readFloat();
			fxaaMul = r.readFloat();
			fxaaShift = r.readFloat();

			bDeselectOnTarget = r.readChar() != 0;
			planetGlowFactor = r.readFloat();

			if(version < 24)
			{
				bStarfield = true;
			}
			else
			{
				bStarfield = r.readBool();
			}
		}
		catch (const std::out_of_range& e)
		{
			LOGWARNING(std::string("out of range exception caught: ") + e.what());
			RestoreDefault();
		}
		catch (const std::exception& e)
		{
			LOGWARNING(e.what());
			RestoreDefault();
		}
	}
	else
	{
		RestoreDefault();
	}
	Apply();
}
void Settings::Save()
{
	LOGWRITE("saving settings");
	FileWriter w(FILEPATH);
	if (w.isOpen())
	{
		w.writeInt(curVersion);
		w.writeChar(bFullscreen);
		w.write<unsigned long long>(MenuScore);
		w.writeString(Name);
		w.writeString(Clan);

		w.writeFloat(volMusic);
		w.writeFloat(volSound);


		for (int i = 0; i < Input::GK_SIZE; i++)
		{
			w.writeChar(inp[i].t);
			w.writeInt(inp[i].code);
		}

		w.writeInt(servers.size());
		for (const auto& i : servers)
		{
			w.writeString(i.nick);
			w.writeString(i.IP);
			w.write<short>(i.port);
		}

		for (auto& a : acvments)
		{
			w.writeChar(a.unlocked);
		}

		// write colors
		for (int i = 1; i < 26; i++)
		{
			w.writeFloat(TeamColors[i].r);
			w.writeFloat(TeamColors[i].g);
			w.writeFloat(TeamColors[i].b);
		}

		w.writeChar(targetLines);
		w.writeFloat(detailLevel);
		w.writeChar(performanceInfo);
		w.writeInt(shaderConfig);

		w.writeFloat(fxaaSpan);
		w.writeFloat(fxaaMul);
		w.writeFloat(fxaaShift);

		w.writeChar(bDeselectOnTarget);
		w.writeFloat(planetGlowFactor);

		w.writeBool(bStarfield);
		LOGWRITE("settings saved");
	}
}
void Settings::RestoreLast()
{
	LoadFromFile();
}
void Settings::RestoreDefault()
{
	LOGWRITE("loading default settings");
	volMusic = 0.7f;
	volSound = 1.0f;
	Name = "";
	Clan = "clanless";
	bFullscreen = false;

	inp[Input::GK_NONE] = { Input::GKstruct::None, 0 };
	inp[Input::GK_SELECT] = { Input::GKstruct::Mouse, Input::Left };
	inp[Input::GK_CLICK] = { Input::GKstruct::Mouse, Input::Left };
	inp[Input::GK_DESELECT] = { Input::GKstruct::Mouse, Input::Right };
	inp[Input::GK_SELECTALL] = { Input::GKstruct::Key, SDL_SCANCODE_A };
	inp[Input::GK_MAKEGROUP] = { Input::GKstruct::Key, SDL_SCANCODE_LCTRL };
	inp[Input::GK_ADDGROUP] = { Input::GKstruct::Key, SDL_SCANCODE_LSHIFT };

	inp[Input::GK_SCROLL] = { Input::GKstruct::Mouse, Input::Middle };
	inp[Input::GK_ZOOMIN] = { Input::GKstruct::Mouse, Input::MouseKey::ScrollDown };
	inp[Input::GK_ZOOMOUT] = { Input::GKstruct::Mouse, Input::MouseKey::ScrollUp };

	inp[Input::GK_CHAT] = { Input::GKstruct::Key, SDL_SCANCODE_T };
	inp[Input::GK_PLAYERLIST] = { Input::GKstruct::Key, SDL_SCANCODE_TAB };

	inp[Input::GK_FILTERENT] = { Input::GKstruct::Key, SDL_SCANCODE_Q };
	inp[Input::GK_FILTERBOMB] = { Input::GKstruct::Key, SDL_SCANCODE_W };
	inp[Input::GK_FILTERSPEED] = { Input::GKstruct::Key, SDL_SCANCODE_E };
	inp[Input::GK_FILTERSABO] = { Input::GKstruct::Key, SDL_SCANCODE_R };
	inp[Input::GK_PLANDEFENSE] = { Input::GKstruct::Key, SDL_SCANCODE_D };

	inp[Input::GK_TURBO] = { Input::GKstruct::Key, SDL_SCANCODE_S };

	MenuScore = 0;
	servers.clear();

	for (auto& a : acvments)
	{
		a.unlocked = false;
	}

	LoadDefaultColors();

	targetLines = true;
	detailLevel = 0.5f;
	performanceInfo = false;
	shaderConfig = 0;

	fxaaSpan = 5.7f;
	fxaaMul = 0.0f;
	fxaaShift = 0.5f;

	bDeselectOnTarget= true;
	planetGlowFactor = 0.4f;

	bStarfield = true;
}
Input::GKstruct Settings::GetGamekey(Input::GameKey g)
{
	assert(g < Input::GK_SIZE);
	return inp[g];
}
void Settings::SetGamekey(Input::GameKey g, Input::GKstruct s)
{
	assert(g < Input::GK_SIZE);
	inp[g] = s;
}
void Settings::Apply()
{
#ifdef SETTINGS_FULL
	LOGWRITE("applying settings");
	Sound::SetMusicVolume(volMusic);
	Sound::SetSoundVolume(volSound);
#endif
}
float Settings::GetVolMusic()
{
	return volMusic;
}
float Settings::GetVolSound()
{
	return volSound;
}
void Settings::SetVolMusic(float NewVolMusic)
{
	volMusic = NewVolMusic;
#ifdef SETTINGS_FULL
	Sound::SetMusicVolume(volMusic);
	if (volMusic == 1.0f)
	{
		Settings::UnlockAchievement(Settings::A_Music);
	}
#endif
}
void Settings::SetVolSound(float NewVolSound)
{
	volSound = NewVolSound;
#ifdef SETTINGS_FULL
	Sound::SetSoundVolume(volSound);
#endif
}
std::string Settings::GetName()
{
	return Name;
}
std::string Settings::GetClan()
{
	return Clan;
}
void Settings::SetName(std::string NewName)
{
	Name = NewName;
}
void Settings::SetClan(std::string NewClan)
{
	Clan = NewClan;
}
bool Settings::GetFullscreen()
{
	return bFullscreen;
}
void Settings::SetFullscreen(bool bNewFullscreen)
{
	bFullscreen = bNewFullscreen;
#ifdef SETTINGS_FULL
	Window::SetFullscreen(bNewFullscreen);
#endif
}
Input::GameKey Settings::MouseToGamekey(Input::MouseKey k)
{
	for (int i = 0; i < Input::GameKey::GK_SIZE; i++)
	{
		if (inp[i].t == Input::GKstruct::Mouse)
		{
			if (inp[i].code == k)
				return (Input::GameKey)i;
		}
	}
	return Input::GameKey::GK_NONE;
}
Input::GameKey Settings::KeyToGamekey(int code)
{
	for (int i = 0; i < Input::GameKey::GK_SIZE; i++)
	{
		if (inp[i].t == Input::GKstruct::Key)
		{
			if (inp[i].code == code)
				return (Input::GameKey)i;
		}
	}
	return Input::GameKey::GK_NONE;
}
unsigned long long Settings::GetMenuScore()
{
	return MenuScore;
}
void Settings::SetMenuStore(unsigned long long val)
{
	MenuScore = val;
}

std::vector<Settings::ServerInfo> Settings::GetServerInfos()
{
	return servers;
}
void Settings::SetServerInfos(std::vector<ServerInfo>&& inf)
{
	servers = std::move(inf);
}

const std::vector< Settings::Achievement >& Settings::GetAchievements()
{
	return acvments;
}

bool Settings::UnlockAchievement(Acvs a)
{
	assert(a < Acvs::A_SIZE);

	if (!acvments[a].unlocked)
	{
		acvments[a].unlocked = true;
		Save();

#ifdef SETTINGS_FULL
		Window::DisplayAchievement(a);
#endif
		return true;
	}
	else
	{
		return false;
	}
}

Color Settings::GetColor(byte team)
{
	if (team < 26)
		return TeamColors[team];
	else
		return Color::White();
}
void Settings::SetColor(byte team, Color c)
{
	if (team < 26 && team > 0)
	{
		TeamColors[team] = c;
	}
}

bool Settings::TargetLinesEnabled()
{
	return targetLines;
}
void Settings::SetTargetLines(bool b)
{
	targetLines = b;
}
void Settings::SetDetailLevel(float l)
{
	detailLevel = tool::clamp(l, 0.0f, 1.0f);
}
float Settings::GetDetailLevel()
{
	return detailLevel;
}
void Settings::SetPerformanceInfo(bool b)
{
	performanceInfo = b;
}
bool Settings::GetPerformanceInfo()
{
	return performanceInfo;
}
unsigned int Settings::GetShader()
{
	return shaderConfig;
}
void Settings::SetShader(unsigned int b)
{
	shaderConfig = b;
}

bool Settings::GetStarfield()
{
	return bStarfield;
}

void Settings::SetStarfield(bool val)
{
	bStarfield = val;
}

float Settings::GetFXAASpan()
{
	return fxaaSpan;
}
float Settings::GetFXAAMul()
{
	return fxaaMul;
}
float Settings::GetFXAAShift()
{
	return fxaaShift;
}
void Settings::SetFXAASpan(float f)
{
	fxaaSpan = f;
}
void Settings::SetFXAAMul(float f)
{
	fxaaMul = f;
}
void Settings::SetFXAAShift(float f)
{
	fxaaShift = f;
}
bool Settings::PlanetGlowEnabled()
{
	return detailLevel > 0.4f && planetGlowFactor > 0.0f;
}
bool Settings::DeselectOnTargetSelect()
{
	return bDeselectOnTarget;
}
void Settings::SetDeselectOnTargetSelect(bool b)
{
	bDeselectOnTarget = b;
}
float Settings::GetPlanetGlowFactor()
{
	return planetGlowFactor;
}
void Settings::SetPlanetGlowFactor(float f)
{
	planetGlowFactor = f;
}