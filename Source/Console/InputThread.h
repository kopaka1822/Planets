#pragma once
#include "Console.h"
#include "../Utility/Thread.h"
#include "../Utility/LockGuard.h"
#include "../System/System.h"
#include <vector>
#include <string>

class InputThread : public Thread
{
	enum class state
	{
		waitForInput,
		waitForBufferEmpty,
		forceStop
	};
public:
	InputThread()
		:
		Thread()
	{}
	virtual ~InputThread()
	{
		ClearMessageAndQuit();
		Thread::Detach();
		// Detach Thread because we dont want to wait for user input
	}
	bool HasMessage() const
	{
		return message.length() != 0;
	}
	std::string GetMessage()
	{
		LockGuard g(muMsg);
		std::string mes = message;
		g.Unlock();

		return mes;
	}
	void ClearMessage()
	{
		LockGuard g(muMsg);
		message.clear();
		g.Unlock();
	}
	void ClearMessageAndQuit()
	{
		LockGuard g(muMsg);
		curState = state::forceStop;
		message.clear();
		g.Unlock();
	}
protected:
	virtual int ThreadProc() override final
	{
		try
		{
			while (curState == state::waitForInput)
			{
				// retrieve input
				std::string res = Console::ReadString();

				if (res.length())
				{
					LockGuard g(muMsg);
					message = res;
					if (curState != state::forceStop)
						curState = state::waitForBufferEmpty;

					g.Unlock();

					// wait till buffer is empty
					while (!message.empty())
					{
						System::Sleep(100);
					}

					// now buffer is empty
					LockGuard g2(muMsg);
					if (curState != state::forceStop)
						curState = state::waitForInput;
					g2.Unlock();
				}
			}
		}
		catch (const std::exception& e)
		{
			SetThreadError(e.what());
		}
		catch (...)
		{
			SetThreadError("unknown input thread error");
		}

		return 0;
	}

private:
	std::string message;
	Mutex muMsg;

	state curState = state::waitForInput;
};