#include "Console\Console.h"
#include "Framework\Settings.h"
#include <list>

/*
	Argumet1:
	-r
	-w


	Read:
	--username (string)
	--clan (string)
	--fullscreen (bool) -> true false
	--menuscore (ull)
	--sound (float)
	--music (float)
	--shader
		-c (int) -> Shadercount
		-n NUMBER (string) -> Shadername (0 - Shadercount-1)
		-v NUMBER (bool) -> Disabled/Enabled

	Write:
	--username
	--clan
	--fullscreen
	--menuscore
	--sound
	--music
	--shader
		-v NUMBER BOOLEAN -> Set Shader[NUMBER] = BOOLEAN
*/

std::string Pop(std::list<std::string>& list)
{
	std::string a = list.front();
	list.pop_front();
	return a;
}

void HandleRead(std::list< std::string >& args)
{
	if (args.size())
	{
		std::string a = Pop(args);
		if (a == "--username")
		{
			Console::Write(Settings::GetName());
		}
		else if (a == "--clan")
		{
			Console::Write(Settings::GetClan());
		}
		else if (a == "--fullscreen")
		{
			Console::Write(Settings::GetFullscreen());
		}
		else if (a == "--menuscore")
		{
			Console::Write(std::to_string(Settings::GetMenuScore()));
		}
		else if (a == "--sound")
		{
			Console::Write(Settings::GetVolSound());
		}
		else if (a == "--music")
		{
			Console::Write(Settings::GetVolMusic());
		}
		else if (a == "--shader")
		{
			if (args.size())
			{
				std::string v2 = Pop(args);
				if (v2 == "-c")
				{
					Console::Write(3);
				}
				else
				{
					if (args.size())
					{
						std::string v3 = Pop(args);
						int num = std::stoi(v3);

						if (v2 == "-n")
						{
							switch (num)
							{
							case 0:
								Console::Write("Radial_Shader");
								break;
							case 1:
								Console::Write("FXAA_Shader");
								break;
							case 2:
								Console::Write("Storybook_Shader");
								break;
							default:
								Console::Write("INVALID");
							}
						}
						else if (v2 == "-v")
						{
							bool en = false;
							switch (num)
							{
							case 0: // Radial
								if (Settings::GetShader() & 1)
									en = true;
								break;
							case 1: // FXAA
								if (Settings::GetShader() & 4)
									en = true;
								break;
							case 2: // Storybook
								if (Settings::GetShader() & 2)
									en = true;
								break;
							}

							Console::Write(en);
						}
						
					}
				}
			}
		}
	}
}

void HandleWrite(std::list< std::string >& args)
{
	if (args.size() >= 2)
	{
		std::string a = Pop(args);
		std::string val = Pop(args);

		if (a == "--username")
		{
			Settings::SetName(val);
		}
		else if (a == "--clan")
		{
			Settings::SetClan(val);
		}
		else if (a == "--menuscore")
		{
			char* sz = 0;
			unsigned long long score = std::strtoull(val.c_str(), &sz, 10);

			Settings::SetMenuStore(score);
		}
		else if (a == "--sound")
		{
			float v = std::stof(val);

			Settings::SetVolSound(v);
		}
		else if (a == "--music")
		{
			float v = std::stof(val);

			Settings::SetVolMusic(v);
		}
		else if (a == "--fullscreen")
		{
			bool b = false;
			if (val == "true")
				b = true;

			Settings::SetFullscreen(b);
		}
		else if (a == "--shader")
		{
			if (args.size() >= 2)
			{
				std::string val1 = Pop(args);
				std::string val2 = Pop(args);
				int num = std::stoi(val1);
				bool en = false;
				if (val2 == "true")
					en = true;

				unsigned int conf = Settings::GetShader();
				int bit = 0;
				switch (num)
				{
				case 0: // Radial
					bit = 1;
					break;
				case 1: // FXAA
					bit = 4;
					break;
				case 2: // Storybook
					bit = 2;
					break;
				}

				if (en)
				{
					conf |= bit;
				}
				else
				{
					conf &= ~bit;
				}
				Settings::SetShader(conf);
			}
		}
	}
}

int main(int argc, char** argv)
{
	std::list< std::string > arguments;
	
	// ignore filename
	for (int i = 1; i < argc; i++)
		arguments.push_back(argv[i]);

#if _DEBUG
	// push some random arguments
	arguments.push_back("-r");
	arguments.push_back("--sound");
	arguments.push_back("--music");
#endif

	// Load Settings
	Settings::Initialize();

	if (arguments.size())
	{
		std::string a = arguments.front();
		arguments.pop_front();

		try
		{

			if (a == "-r")
			{
				while (arguments.size())
				{
					HandleRead(arguments);
					Console::Write(" ");
				}
			}
			else if (a == "-w")
			{
			
					while (arguments.size())
						HandleWrite(arguments);
			
					Settings::Save();
			}

		}
		catch (const std::exception& e)
		{
			Console::Write(e);
		}
	}

	return 0;
}