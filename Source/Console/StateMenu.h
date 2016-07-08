#pragma once
#include "LevelToolApp.h"

class StateMenu : public ConsoleState
{
public:
	StateMenu()
		:
		menu("Menu")
	{
		menu.AddLine("open savedata", LevelToolApp::readSave);
		menu.AddLine("analyse savedatas", LevelToolApp::analyseSaves);
		menu.AddLine("quit", LevelToolApp::quit);
	}
	virtual int Run() override
	{
		return menu.Display();
	}
private:
	ConsoleMenu menu;
};