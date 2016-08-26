#pragma once
#include "Map.h"
#include "../Server/Client.h"
#include "NetClock.h"

class RemoteMap : public Map
{
public:
	RemoteMap(Client& cl, int nPlay, const std::vector< MapLoader::MapPlanet >& planets, const std::vector< MapLoader::MapEntity >& entities,
		float width, float height, float gameTime, Map::GameType ty, std::vector< byte > clns);
	virtual ~RemoteMap();
	virtual bool Select(PointF center, float r2, byte team) override;
	virtual void SelectGroup(byte team, int group) override;
	virtual void DeleteGroup(byte team, int group) override;
	virtual bool Click(PointF pt, byte team) override;
	virtual void ClickRight(byte team) override;
	virtual void Update(float dt) override;
	virtual byte GameEnd() const override;
	virtual void SetAllPlanetsOnDefense(byte team) override;
	virtual bool GameStart() const override
	{
		return bGameStart;
	}
	virtual float GetGameTime() const override final
	{
		return clock.GetTimeSecond();
	}
	virtual void AddToGroup(byte team, int group) override;
	virtual void MakeGroup(byte team, int group) override;
	virtual void SelectAll(byte team) override;
	virtual void SetPlanetSpawnType(byte team, MapObject::EntityType t) override;
	virtual bool FilterEntityType(byte team, MapObject::EntityType et) override;
	virtual void SelectAllEntityType(byte team, MapObject::EntityType et) override;
	virtual void AddPacket(DataContainer&& con) override;
private:
	Client& serv;
	byte winner = 0;
	NetClock clock;
	bool bGameStart = false;

	
private:
	void SetEntPositionRemote(MapEntity& curEnt, const float dt)
	{
		const PointF& pos = curEnt.getPos();
		const VectorF& vel = curEnt.getVel();

		if (GetColEnt(pos, curEnt.getID(),curEnt.getTeam()) != nullptr)
		{
			// probably due to network bug
			// force set to new position
			if (GetColPlan(pos + vel) == nullptr)
			{
				curEnt.updatePosition(dt);
				return;
			}
		}

		SetEntPosition(curEnt, dt);
	}
	void ReceiveData(bool& updatePositions);
	void HandleSpawn(ContainerReader& r);
	void HandleGameSelect(ContainerReader& r);
	void HandleGameClick(ContainerReader& r);
	void HandleEntPosUpdate(ContainerReader& r);
	void UpdateMovement(float dt);
	void HandleDie(ContainerReader& r);
	void HandlePlanCap(ContainerReader& r);
	void HandlePlanUpd(ContainerReader& r);
	void HandleGameEnd(ContainerReader& r);
	void HandleSynch(ContainerReader& r);
	void ClearUpdateTime();
	void UpdateMovementServer();
	void HandleSurrender(ContainerReader& r);
	void HandleClanChange(ContainerReader& r);
	void HandlePlanetSpawntype(ContainerReader& r);
	void HandleSelectAllEntityType(ContainerReader& r);
	void HandleFilterEntityType(ContainerReader& r);
	void HandlePlanDefend(ContainerReader& r);
private:
	std::vector< DataContainer > dataList;
};