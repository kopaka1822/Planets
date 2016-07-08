#pragma once
#include "LevelToolApp.h"
#include "../Game/LevelSavedata.h"
#include "../Utility/Directory.h"

class StateSaveAnalyse : public ConsoleState
{
public:
	StateSaveAnalyse()
	{

	}
	virtual ~StateSaveAnalyse()
	{

	}
	virtual int Run() override
	{
		Console::WriteLine("please put saves in the saves directory and press enter");
		Console::Pause();

		auto names = GetDirectoryFilenames("saves/");

		bestTimes.assign(12, -1.0f);
		avgTimes.assign(12, 0.0f);
		avgCount.assign(12, 0);
		for (const auto& s : names)
		{
			try
			{
				LevelSavedata save("saves/" + s, 0, true);

				for (int i = 0; i < 12; i++)
				{
					float f = save.GetTimes()[i];
					if (f > 0.0f)
					{
						if (bestTimes[i] > 0.0f)
							bestTimes[i] = std::min(f, bestTimes[i]);
						else
							bestTimes[i] = f;

						avgTimes[i] += f;
						avgCount[i] ++;
					}
				}
			}
			catch (const std::out_of_range&)
			{ 
			}
		}

		// Print stats
		Console::EndLine();
		Console::WriteLine("BEST:");
		int i = 1;
		for (const auto& t : bestTimes)
		{
			Console::Write(i++);
			Console::Write(": ");
			Console::WriteLine(t);
		}

		Console::EndLine();
		Console::WriteLine("AVERAGE:");

		i = 1;
		for (const auto& t : avgTimes)
		{
			Console::Write(i++);
			Console::Write(": ");
			Console::WriteLine(t / std::max(float(avgCount[i - 2]), 1.0f));
		}

		Console::Pause();

		return LevelToolApp::menu;
	}
private:
	std::vector<float> bestTimes;
	std::vector<float> avgTimes;
	std::vector<int> avgCount;
};