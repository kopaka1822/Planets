#pragma once
#include "Server.h"
#include "../Utility/Thread.h"
#include "../Utility/Exception.h"

class GameServer : public Thread
{
protected:
	enum class status
	{
		Retry,
		Next,
		Abort
	};
public:
	virtual ~GameServer()
	{}
	virtual void Start() = 0;
	virtual bool isFinished() = 0;
	virtual bool isJoinable() = 0;
	virtual void Close(){}
	virtual PORT GetPort() const = 0;
	virtual size_t GetModi() const = 0;

protected:
	// Default Text _Output
	virtual void Print(const std::string& s)
	{
		printf("%s\n", s.c_str());
	}
	// dt in seconds
	virtual status UpdateLobby(float dt) = 0;
	virtual status UpdateGame(float dt) = 0;
	virtual status CloseGame(){ return status::Retry; }


	virtual int ThreadProc() override
	{
		try
		{
			status curStatus = status::Retry;
			Timer tmr;
			tmr.StartWatch();


			// Lobby
			Print("entering lobby");
			while (curStatus == status::Retry && bRun)
			{
				UpdateServer();

				float dt = tmr.GetTimeSecond();
				tmr.StartWatch();

				curStatus = UpdateLobby(dt);

				int slTime = 100 - int(tmr.GetTimeMilli());
				if (slTime > 0)
					System::Sleep(slTime);
			}

			// Game
			if (curStatus == status::Next && bRun)
			{
				Print("starting game");
				curStatus = status::Retry;
			}

			while (curStatus == status::Retry && bRun)
			{
				UpdateServer();

				float dt = tmr.GetTimeSecond();
				tmr.StartWatch();

				curStatus = UpdateGame(dt);

				int slTime = 30 - int(tmr.GetTimeMilli());
				if (slTime > 0)
					System::Sleep(slTime);
			}

			Print("closing game");

			// End (max. 10 seconds)
			for (int i = 0; i < 100 && bRun; i++)
			{
				UpdateServer();

				if (pServer->GetNumConnected() == 0)
				{
					bRun = false;
					Print("all players disconnected");
					break;
				}

				curStatus = CloseGame();
				System::Sleep(100);
			}
		}
		catch (const std::exception& e)
		{
			Print("Game Error " + std::string(e.what()));
			return 1;
		}
		catch (...)
		{
			Print("Unknown Game Error");
			return -1;
		}
		return 0;
	}

private:
	void UpdateServer()
	{
		pServer->Update();

		if (pServer->HasThreadError())
		{
			throw Exception(pServer->GetThreadError());
		}
	}
protected:
	std::unique_ptr< Server > pServer;
	bool bRun = true;
};
