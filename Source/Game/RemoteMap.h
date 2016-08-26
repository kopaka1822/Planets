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
	virtual bool select(PointF center, float r2, byte team) override;
	virtual void selectGroup(byte team, int group) override;
	virtual void deleteGroup(byte team, int group) override;
	virtual bool setTarget(PointF pt, byte team) override;
	virtual void deselectTarget(byte team) override;
	virtual void update(float dt) override;
	virtual byte gameEnd() const override;
	virtual void setAllPlanetsOnDefense(byte team) override;
	virtual bool gameStart() const override
	{
		return m_bGameStart;
	}
	virtual float getGameTime() const override final
	{
		return m_clock.getTimeSecond();
	}
	virtual void addToGroup(byte team, int group) override;
	virtual void makeGroup(byte team, int group) override;
	virtual void selectAll(byte team) override;
	virtual void setPlanetSpawnType(byte team, MapObject::EntityType t) override;
	virtual bool filterEntityType(byte team, MapObject::EntityType et) override;
	virtual void selectAllEntityType(byte team, MapObject::EntityType et) override;
	virtual void addPacket(DataContainer&& con) override;
private:
	Client& m_serv;
	byte m_winner = 0;
	NetClock m_clock;
	bool m_bGameStart = false;

	
private:
	void setEntPositionRemote(MapEntity& curEnt, const float dt)
	{
		const PointF& pos = curEnt.getPos();
		const VectorF& vel = curEnt.getVel();

		if (getColEnt(pos, curEnt.getID(),curEnt.getTeam()) != nullptr)
		{
			// probably due to network bug
			// force set to new position
			if (getColPlan(pos + vel) == nullptr)
			{
				curEnt.updatePosition(dt);
				return;
			}
		}

		setEntPosition(curEnt, dt);
	}
	void receiveData(bool& updatePositions);
	void handleSpawn(ContainerReader& r);
	void handleGameSelect(ContainerReader& r);
	void handleGameClick(ContainerReader& r);
	void handleEntPosUpdate(ContainerReader& r);
	void updateMovement(float dt);
	void handleDie(ContainerReader& r);
	void handlePlanCap(ContainerReader& r);
	void handlePlanUpd(ContainerReader& r);
	void handleGameEnd(ContainerReader& r);
	void handleSynch(ContainerReader& r);
	void clearUpdateTime();
	void updateMovementServer();
	void handleSurrender(ContainerReader& r);
	void handleClanChange(ContainerReader& r);
	void handlePlanetSpawntype(ContainerReader& r);
	void handleSelectAllEntityType(ContainerReader& r);
	void handleFilterEntityType(ContainerReader& r);
	void handlePlanDefend(ContainerReader& r);
private:
	std::vector< DataContainer > m_dataList;
};