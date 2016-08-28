#pragma once
#include "LocalMap.h"
#include "../Utility/DataContainer.h"
#include "../Server/Server.h"
#include "NetClock.h"

class LocalMapServer : public LocalMap
{
public:
	//structures
public:
	LocalMapServer(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns, 
		Server& serv, float width, float height,
		std::function<byte(int)> GetPlayerTeam, Map::GameType ty, std::vector< TeamID > clns);
	virtual ~LocalMapServer();

	std::vector< DataContainer > GetStartData();

	virtual void update(float dt) override;
	virtual void deselectTarget(TeamID team) override;
	virtual bool setTarget(PointF pt, TeamID team) override;
	virtual TeamID gameEnd() const override
	{
		return winner;
	}
	void SetStartClock(float startsec)
	{
		clock.setStartTime(startsec);
	}

	virtual void addToGroup(TeamID team, GroupID group) override;
	virtual void makeGroup(TeamID team, GroupID group) override;
	virtual void selectGroup(TeamID team, GroupID group) override;
	virtual void deleteGroup(TeamID team, GroupID group) override;
	virtual void selectAll(TeamID team) override;
	virtual bool filterEntityType(TeamID team, MapObject::EntityType et) override;
	virtual void selectAllEntityType(TeamID team, MapObject::EntityType et) override;

	bool HandleSurrender(byte team);
	float GetAvgUpdTime()
	{
		float avg = sumTime / float(runs);
		runs = 0;
		sumTime = 0.0f;
		return avg;
	}
	virtual void addPacket(DataContainer&& con) override
	{
		inData.push_back(std::move(con));
	}

	size_t CountUnits(TeamID team)
	{
		if (unsigned(team - 1) >= m_nPlayers)
			return 0;

		return m_ents[team - 1].length();
	}
	TeamID GetNPlayers()
	{
		return m_nPlayers;
	}
	virtual bool gameStart() const override
	{
		return bGameStart;
	}
protected:
	virtual void Event_PlanetCaptured(PlanetID pID, TeamID newTeam, TeamID oldTeam, const MapEntity*) override;
private:
	Server& serv;
	NetClock clock;
	bool bGameStart = false;

	std::function<byte(int)> GetPlayerTeam;

	std::vector< DataContainer > inData;
private: //functions
	unsigned int nEntities() const
	{
		unsigned int c = 0;
		for (size_t i = 0; i < m_nPlayers; ++i)
		{
			c += m_ents[i].length();
		}
		return c;
	}
	void UpdatePlanets(float dt);
	void ReceiveData();
	void HandleClientSelect(ContainerReader& r, byte team);
	void HandleClientClanRequest(byte asking, byte team);
	void HandleClientClanDestroy(byte asking, byte team);
	void HandleClientSpawntype(ContainerReader& r, byte team);
	void HandleClientPlanDefense(ContainerReader& r, byte team);
	void UpdateMovement(float dt, const float curTime);
	void AttackNearby(MapEntity& curEnt);
	void UpdateDeath();
	void SendWinnerMessage();
	void SynchClock(float curTime);

	bool isAlive(byte team) const;
	float CalcMoveResendTime() const;

	void SendAllyRequest(byte from, byte to);
	void SendAllyFormed(byte t1, byte t2);
	void SendAllyResolve(byte t1, byte t2);
	void SendAllyDeny(byte from, byte to);

	void SendClanUpdate(byte t1, byte t2);

	bool AllyAllowed(byte t1, byte t2) const
	{
		if (t1 == t2)
			return false;

		if (CountAllies(t1) >= 2)
			return false;

		if (CountAllies(t2) >= 2)
			return false;

		return true;
	}
	int CountAllies(byte team) const
	{
		int count = 0;
		for (const auto& e : m_clanInfo[team - 1])
			if (e == ClanInfo::Ally) count++;

		return count;
	}
private:
	//server constants
	const unsigned int maxEntLen;
	byte winner = 0;

	// timer
	float lastClock = 0.0f;
	float moveTime = 0.0f;
	float planTime = 0.0f;

	// debugging
	float sumTime = 0.0f;
	unsigned int runs = 0;
	TeamID lastTeamUpdate = 0;

	const unsigned int MAX_PACKSIZE;// = 4000;//1400; // more than 1467 does not work on jans server..

	const std::vector< TeamID > startTeams; // to send map data
};