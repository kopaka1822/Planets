#pragma once
#include "ConsoleState.h"
#include <memory>

class ConsoleApp
{
public:
	ConsoleApp()
	{}
	virtual ~ConsoleApp()
	{}
	void Run()
	{
		try
		{
			curState = GetInitState();

			while (curState)
			{
				int next = curState->Run();

				curState = GetState(next);
			}
		}
		catch (const std::exception& e)
		{
			Console::Write(e);
			Console::Pause();
		}
	}
protected:
	virtual std::unique_ptr< ConsoleState > GetInitState() = 0;
	virtual std::unique_ptr< ConsoleState > GetState(int code) = 0;
private:
	std::unique_ptr< ConsoleState > curState;
};