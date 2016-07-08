#include "System/System.h"
#include "Server/MultiGameLobby.h"
#include "Utility/Timer.h"
#include <stdio.h>
#include "Server/UDPServer.h"
#include "Console/InputThread.h"

int main(int argc, char** argv)
{
	try
	{
        assert(sizeof(int) == 4);

		System::Init();

		InputThread is;

		MultiGameLobby lobby;
		lobby.LoadSettings();
		lobby.Init(40); // lobby player size

		// start input stream for parralel input
		is.Begin();

		Timer t;
		t.StartWatch();
		while (1)
		{
			float dt = t.GetTimeSecond();
			t.StartWatch();

			lobby.Update(dt);

			// thread friendly
			int sleepTime = 250 - (int)t.GetTimeMilli();
			if (sleepTime > 0)
				System::Sleep(sleepTime);

			if (is.HasThreadError())
				throw Exception(is.GetThreadError());

			if (is.HasMessage())
			{
				auto mes = is.GetMessage();

				if (mes == "-quit")
				{
					is.ClearMessageAndQuit();
					Console::WriteLine("quitting...");
					is.Join();
					break;
				}
				else if (mes == "-reload")
				{
					lobby.ReloadMaps();
				}
				else
				{
					// available commands:
					Console::WriteLine("comands:");
					Console::WriteLine("-quit \t\t quit server");
					Console::WriteLine("-reload \t reload maps");
				}

				is.ClearMessage();
			}
		}
	}
	catch (const std::exception& e)
	{
		printf("error: %s\n", e.what());
	}

	System::Shutdown();

	// wait to see the error
	return 0;
}
