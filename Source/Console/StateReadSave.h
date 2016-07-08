#pragma once
#include "LevelToolApp.h"
#include "../Utility/DialogOpenFile.h"
#include "../Game/LevelSavedata.h"

class StateReadSave : public ConsoleState
{
public:
	StateReadSave()
	{

	}
	virtual ~StateReadSave()
	{

	}
	virtual int Run() override
	{
		DialogOpenFile dia("sav");
		dia.Show();

		if (dia.IsSuccess())
		{
			auto name = dia.GetName();
			try
			{
				LevelSavedata save(name, 0, true);

				// read things
				Console::WriteLine("Times:");

				int i = 1;
				for (const auto& t : save.GetTimes())
				{
					Console::Write(i++);
					Console::Write(": ");
					Console::WriteLine(t);
				}


				Console::Pause();
			}
			catch (const std::out_of_range& e)
			{
				Console::Write(e);
			}
		}

		return LevelToolApp::menu;
	}
private:

};