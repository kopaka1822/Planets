#include "MultiGameLobby.h"
#include "../Utility/Directory.h"
#include "../Game/PacketTypes.h"
#include "GameServerNormal.h"
#include "ServerOutput.h"
#include "GameServerNormalDynMap.h"
#include <algorithm>
#include "../Console/Console.h"
#include "../Utility/FileReader.h"
#include "../Utility/FileWriter.h"

static const char* FILEPATH = "data/multiserver.dat";


MultiGameLobby::MultiGameLobby()
	:
	chatHandler(*this)
{
	// text
	startPageLeft.push_back(PageItem(PageObject::LabelBig,
		PageColor(255, 255, 255).color, "Select Mode:"));
	startPageLeft.push_back(PageItem(PageObject::Button,
		PageID::BTN_UNHOLY, "Unholy Alliance"));
	startPageLeft.push_back(PageItem(PageObject::Button,
		PageID::BTN_ALLIANCE, "Alliance"));
	startPageLeft.push_back(PageItem(PageObject::Button,
		PageID::BTN_ALLVALL, "All vs All"));


	chatHandler.RegisterCommand("ping", std::mem_fn(&MultiGameLobby::HandlePing), "shows ping");

	chatHandler.GenerateHelpMessage();
}

void MultiGameLobby::LoadSettings()
{
	if (!LoadFromFile())
		CreateSaveFile();

	LoadMaps();
	OrderMaps();
}

bool MultiGameLobby::LoadFromFile()
{
	Console::WriteLine("loading settings");

	FileReader r(FILEPATH);

	if (!r.isOpen())
		return false; // file does not exist

	try
	{
		maxGames = r.readInt();
		maxPackageSize = r.readInt();
	}
	catch (const std::out_of_range& e)
	{
		Console::Write(e);
		return false;
	}

	if (maxGames <= 0)
		return false;

	Console::Write("number of parallel games: ");
	Console::WriteLine(maxGames);

	Console::Write("maximal UDP package size: ");
	Console::WriteLine(maxPackageSize);

	return true;
}
void MultiGameLobby::CreateSaveFile()
{
	Console::WriteLine("no valid settings found");

	Console::Write("maximal number of parallel games: ");
	maxGames = Console::ReadInt();
	if (maxGames <= 0)
		throw Exception("settings invalid argument");

	Console::Write("maximal size of UDP packages (max is 4000): ");
	maxPackageSize = Console::ReadInt();
	if (maxPackageSize <= 200)
		throw Exception("settings invalid argument (too small)");
	maxPackageSize = std::min(4000, maxPackageSize);

	FileWriter w(FILEPATH);

	if (w.isOpen())
	{
		w.writeInt(maxGames);
		w.writeInt(maxPackageSize);
	}
}

void MultiGameLobby::LoadMaps()
{
	maps.clear();

	std::vector< std::string > files = GetDirectoryFilenames("data/maps");

	if (files.size() == 0)
		throw Exception("no *.map files in: data/maps/");

	Console::Write("found ");
	Console::Write((int)files.size());
	Console::WriteLine(" maps");

	for (const auto& s : files)
	{
		MapLoader loader(std::string("data/maps/") + s);
		Console::EndLine();
		Console::Write("filename: ");
		Console::WriteLine(s);

		if (loader.isValid())
		{
			Console::Write("planets: ");
			Console::WriteLine((int)loader.GetPlanets().size());
			Console::Write("teams: ");
			Console::WriteLine(loader.GetPlayers());

			maps.push_back(std::move(loader));
		}
		else
		{
			Console::WriteLine("invalid map!");
		}
	}

	if (maps.size() == 0)
		throw Exception("no valid maps");
}

void MultiGameLobby::OrderMaps()
{
	// find max player size
	// Order maps
	mapsOrdered.clear();

	maxPlayerSize = 2;
	for (const auto& m : maps)
	{
		if (!m.HasTeamSupport())
		{
			maxPlayerSize = std::max(maxPlayerSize, (int)m.GetPlayers());

			auto v = mapsOrdered.find(m.GetPlayers());
			if (v == mapsOrdered.end())
			{
				// create
				mapsOrdered[m.GetPlayers()] = std::vector< MapLoader >();
				mapsOrdered[m.GetPlayers()].push_back(m);
			}
			else
			{
				v->second.push_back(m);
			}
		}
	}

	Console::Write("maximal player size is : ");
	Console::WriteLine(maxPlayerSize);

	// cheack if everything between 2 and maxsize is available
	for (int i = 2; i <= maxPlayerSize; i++)
	{
		if (mapsOrdered.find(i) == mapsOrdered.end())
			throw Exception((std::string("please provide a map for ")
			+ std::to_string(i) + std::string(" players (no teams)")).c_str());
	}
	
	Console::WriteLine("loading m_team maps");
	mapsTeams.clear();

	for (const auto& m : maps)
	{
		if (m.HasTeamSupport())
		{
			if (mapsTeams.size() == 0)
			{
				maxTeamSize = m.GetPlayers();
				minTeamSize = m.GetPlayers();
			}

			maxTeamSize = std::max(maxTeamSize, (int)m.GetPlayers());
			minTeamSize = std::min(minTeamSize, (int)m.GetPlayers());

			auto v = mapsTeams.find(m.GetPlayers());
			if (v == mapsTeams.end())
			{
				// create
				mapsTeams[m.GetPlayers()] = std::vector< MapLoader >();
				mapsTeams[m.GetPlayers()].push_back(m);
			}
			else
			{
				v->second.push_back(m);
			}
		}
	}
	if (mapsTeams.size() == 0)
		throw Exception("no maps for alliance");

	Console::Write("minimal player size for alliance: ");
	Console::WriteLine(minTeamSize);
	Console::Write("maximal player size for alliance: ");
	Console::WriteLine(maxTeamSize);

	for (int i = minTeamSize; i <= maxTeamSize; i++)
	{
		if (mapsTeams.find(i) == mapsTeams.end())
			throw Exception((std::string("please provide a TEAM map for ")
			+ std::to_string(i) + std::string(" players")).c_str());
	}

	// Unholy alliance
	if (maxPlayerSize < 4)
		throw Exception("please provide a map for 4 players");

	Console::WriteLine("succesfully loaded maps!");
}

void MultiGameLobby::Init(int maxPlayerLobby)
{
	// Start Server
	Console::WriteLine("starting server");
	NetServer::SetMaxPackSize(maxPackageSize);
	pServer = std::unique_ptr<Server>(new Server(7123, maxPlayerLobby));

	servers.clear();
	for (int i = 0; i < maxGames; i++)
		servers.push_back(std::unique_ptr<GameServer>());

	PlayerInfo i;
	ResetPlayerInfo(&i);
	players.assign(maxPlayerLobby, i);
}

void MultiGameLobby::Update(float dt)
{
	pServer->Update();
	for (const auto& e : pServer->GetEvents())
	{
		switch (e.e)
		{
		case Server::Event::Join:
			GlobalPrint(7123, "player joined");
			players[e.playerID - 1].bConnected = true;
			SendServerInfo(e.playerID);
			break;
		case Server::Event::Disconnect:
			GlobalPrint(7123, "player disconnected");
			ResetPlayerInfo(&players[e.playerID - 1]);
			break;
		case Server::Event::TimedOut:
			GlobalPrint(7123, "player timed out");
			ResetPlayerInfo(&players[e.playerID - 1]);
			break;
		}
	}

	for (const auto& c : pServer->GetData())
	{
		try
		{
			ContainerReader r(c);

			int pid = r.readInt();
			PacketType t = (PacketType)r.readShort();
			switch (t)
			{
			case PacketType::Username:
				if (r.remaining())
				{
					players[pid - 1].name = r.readString();
					GlobalPrint(7123, (std::string("Username of ") + std::to_string(pid)
						+ std::string(" is: ") + players[pid - 1].name).c_str());
				}
				SendStartPage(pid);
				listTime = 3.0f; // resend list
				break;
			case PacketType::ClientButtonpress:
				HandleButton(r, pid);
				break;
			case PacketType::ClientChat:
				HandleChat(r, pid);
				break;
			}
		}
		catch (const std::logic_error&)
		{

		}
	}

	listTime += dt;
	if (listTime > 2.0f)
	{
		listTime = 0.0f;
		UpdateListWithPlayerPings(RIGHT);
	}

	// control servers
	for (auto& s : servers)
	{
		if (s)
		{
			if (s->HasThreadError())
			{
				s->Close();
				s.reset();
				continue;
			}
			if (s->isFinished())
			{
				s->Close();
				s.reset();
			}
		}
	}
}
void MultiGameLobby::ReloadMaps()
{
	// stop all games to prevent crashes
	CloseGames();
	LoadMaps();
	OrderMaps();
}

void MultiGameLobby::HandleChat(ContainerReader& r, int id)
{
	std::string msg = r.readString();
	if (msg.at(0) == '/')
	{
		if (!chatHandler.InpterpretCString(id, msg.c_str() + 1))
		{
			SendServerMessage(id, chatHandler.GetHelpMessage().c_str());
		}
	}
	else
	{
		// TODO check for max lenght
		DataContainer con = pServer->GetConRel();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::GameChat);
		w.writeByte((id % 25) + 1); // m_team color
		w.writeStringNoNull("[");
		w.writeStringNoNull(players[id - 1].name.c_str());
		w.writeStringNoNull("] ");
		w.writeString(msg);

		pServer->SendContainerAllReliable(std::move(con));
	}
}
void MultiGameLobby::SendServerMessage(int id, const char* txt, byte color)
{
	DataContainer con = pServer->GetConRel();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::GameChat);
	w.writeByte(color);
	w.writeStringNoNull("[Server] ");
	w.writeString(txt);

	if (id)
		pServer->SendContainerReliable(std::move(con), id);
	else
		pServer->SendContainerAllReliable(std::move(con));
}
void MultiGameLobby::HandlePing(int id, std::list< std::string > args)
{
	float ping = pServer->GetPing(id);
	std::string msg = "ping: " + std::to_string((int)ping);
	SendServerMessage(id, msg.c_str());
}

void MultiGameLobby::CloseGames()
{
	GlobalPrint(7123, "closing games");
	for (auto& s : servers)
	{
		if (s)
		{
			s->Close();
			s.reset();
		}
	}
}
MultiGameLobby::~MultiGameLobby()
{
	CloseGames();
	
	if (pServer)
	{
		GlobalPrint(7123,"stopping server");
		pServer->StopServer();
		pServer.reset();
	}
}

void MultiGameLobby::RedirectPlayer(int id, size_t modi)
{
	DataContainer con = pServer->GetConRel();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ServerType);
	w.writeInt(Version::GetGameVersion());
	w.writeChar((char)ServerTypes::Redirecting);
	w.writeShort(GetFreeGamePort(modi));
	w.writeShort(0);

	pServer->SendContainerReliable(std::move(con), id);
}
void MultiGameLobby::SendServerInfo(int id)
{
	DataContainer con = pServer->GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ServerType);
	w.writeInt(Version::GetGameVersion());
	w.writeChar((char)ServerTypes::GameSelectLobby);

	pServer->SendContainerReliable(std::move(con), id);
}

PORT MultiGameLobby::GetFreeGamePort(size_t modi)
{
	// is any game joinable?
	for (auto& s : servers)
	{
		if (s)
		{
			if (s->isJoinable() && s->GetModi() == modi)
			{
				return s->GetPort();
			}
		}
	}

	// free slots?
	PORT prt = 7130;
	for (auto& s : servers)
	{
		if (!s)
		{
			switch (modi)
			{
			case (size_t)Map::GameType::AllvAll:
				s = std::unique_ptr< GameServerNormalDynMap >(new GameServerNormalDynMap(*this, 2, maxPlayerSize,
					prt, Map::GameType::AllvAll));
				break;
			case (size_t)Map::GameType::UnholyAlliance:
				s = std::unique_ptr< GameServerNormalDynMap >(new GameServerNormalDynMap(*this, 4, maxPlayerSize,
					prt, Map::GameType::UnholyAlliance));
				break;
			case (size_t)Map::GameType::Allicance:
				s = std::unique_ptr< GameServerNormalDynMap >(new GameServerNormalDynMap(*this, GetMinTeamPlayer(), GetMaxTeamPlayer(),
					prt, Map::GameType::Allicance));
				break;
			default:
				return 0;
			}
			s->Start();

			return s->GetPort();
		}
		prt++;
	}

	// no free slots
	return 0;
}

void MultiGameLobby::SendStartPage(int id)
{
	SendPageHeadline(PAGES::STARTPAGE, "Welcome to Beta Server", id);
	SendPageList(startPageLeft, id, LEFT);
	//SendPageList(startPageRight, id, RIGHT);
}

void MultiGameLobby::SendPageHeadline(PAGES id, const std::string& txt, int player)
{
	DataContainer con = pServer->GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::PageNewSection);
	w.writeInt(id);
	w.writeString(txt.c_str());

	pServer->SendContainerReliable(std::move(con), player);
}

void MultiGameLobby::SendPageList(const std::vector< PageItem >& list, int player, LIST l)
{
	DataContainer con = pServer->GetConRelSmall();
	ContainerWriter w(con);

	if (l == LEFT)
		w.writeShort((short)PacketType::PageUpdateLeft);
	else
		w.writeShort((short)PacketType::PageUpdateRight);

	w.writeInt(list.size());

	for (const auto& e : list)
	{
		w.writeByte((byte)e.type);
		w.writeInt(e.id);
		w.writeString(e.text.c_str());
	}
	if (player != 0)
		pServer->SendContainerReliable(std::move(con), player);
	else
		pServer->SendContainerAllReliable(std::move(con));
}
void MultiGameLobby::HandleButton(ContainerReader& r, int player)
{
	int id = r.readInt();

	switch (id)
	{
	case PageID::BTN_ALLIANCE:
		RedirectPlayer(player, (size_t)Map::GameType::Allicance);
		break;
	case PageID::BTN_ALLVALL:
		RedirectPlayer(player, (size_t)Map::GameType::AllvAll);
		break;
	case PageID::BTN_UNHOLY:
		RedirectPlayer(player, (size_t)Map::GameType::UnholyAlliance);
		break;
	}
}

void MultiGameLobby::UpdateListWithPlayerPings(LIST side)
{
	std::vector< PageItem > lst;
	
	lst.push_back(PageItem(PageObject::Label, PageColor(255, 255, 255).color,
		"Players:"));
	int id = 1;
	for (const auto& p : players)
	{
		if (p.bConnected)
		{
			lst.push_back(PageItem(PageObject::Label, PageColor(255, 255, 255).color,
				p.name + std::string("   ") + std::to_string((int)pServer->GetPing(id))));
		}
		id++;
	}
	if (lst.size() > 1)
		SendPageList(lst, 0, side);
}
