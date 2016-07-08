#pragma once
#include "Console.h"
#include "ConsoleMenu.h"

class ConsoleState
{
public:
	ConsoleState()
	{}
	virtual ~ConsoleState()
	{}
	virtual int Run() = 0;
};