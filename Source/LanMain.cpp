#include <stdio.h>
#include <exception>
#include <conio.h>
#include "System\System.h"
#include "Utility\DialogOpenFile.h"
#include "Game\MapLoader.h"
#include "Game\Map.h"
#include "Server\GameServerNormalFixedMap.h"

int main(int argc, char** argv)
{
	try
	{
		System::Init();
		//Select Map

		DialogOpenFile dialog("map");
		dialog.Show();

		if (!dialog.IsSuccess())
			throw std::exception("no map chosen");

		std::string filename = dialog.GetName();

		MapLoader loader(filename.c_str());
		if (!loader.isValid())
			throw std::exception("invalid map");

		// determine game type
		printf("\nChoose Game Type:\n");
		printf("\ta\tAll vs All\n");
		printf("\tu\tUnholy Alliance\n");
		if (loader.HasTeamSupport())
			printf("\tt\tTeams\n");

		Map::GameType type = Map::GameType::NONE;
		while (type == Map::GameType::NONE)
		{
			if (_kbhit())
			{
				switch (_getch())
				{
				case 'a':
					type = Map::GameType::AllvAll;
					break;
				case 'u':
					type = Map::GameType::UnholyAlliance;
					break;
				case 't':
					if (loader.HasTeamSupport())
						type = Map::GameType::Allicance;
					break;
				}
			}
		}

		printf("Game Type: ");
		switch (type)
		{
		case Map::GameType::AllvAll:
			printf("All vs All\n");
			break;
		case Map::GameType::UnholyAlliance:
			printf("Unholy Alliance\n");
			break;
		case Map::GameType::Allicance:
			printf("Alliance\n");
			break;
		}

		printf("using port 7123\n");
		GameServerNormalFixed game(loader, 7123, type);

		game.Start();

		while (!game.isFinished())
		{
			System::Sleep(500);

			if (_kbhit())
			{
				if (_getch() == 'q')
				{
					printf("quitting...\n");
					break;
				}
			}
		}
		game.Close();
		printf("game closed succesfully!\n");
	}
	catch (const std::exception& e)
	{
		printf("Server Error: %s\n", e.what());
	}
	catch (...)
	{
		printf("Unknown Server Error\n");
	}
	
	System::Shutdown();

	printf("press q to quit\n");
	while (_getch() != 'q');

	return 0;
}