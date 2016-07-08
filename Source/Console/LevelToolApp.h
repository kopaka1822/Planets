#pragma once
#include "ConsoleApp.h"

class LevelToolApp : public ConsoleApp
{
public:
	enum states
	{
		quit,
		menu,
		readSave,
		analyseSaves
	};
public:
	LevelToolApp()
	{}
	virtual ~LevelToolApp()
	{}

	virtual std::unique_ptr< ConsoleState > GetInitState() override;
	virtual std::unique_ptr< ConsoleState > GetState(int code) override;
};