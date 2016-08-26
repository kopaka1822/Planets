#include "RemoteMap.h"
#include "PacketTypes.h"
#include "RemoteEntity.h"
#include "RemotePlanet.h"

RemoteMap::RemoteMap(Client& cl, int nPlay, const std::vector< MapLoader::MapPlanet >& planets, const std::vector< MapLoader::MapEntity >& entities,
	float width, float height, float gameTime, Map::GameType ty, std::vector< byte > clns)
	:
	Map(nPlay, planets.size(),width,height,ty,clns),
	m_serv(cl), m_clock(gameTime)
{
	//Load planets
	for (unsigned int i = 0; i < planets.size(); ++i)
	{
		m_plans.push_back(new RemotePlanet(planets[i], i));
	}

	//set entities
	for (const auto& e : entities)
	{
		unsigned int id = m_ents[e.team - 1].lastID() + 1;
		m_ents[e.team - 1].add(new RemoteEntity(PointF{ e.x, e.y }, e.team, MapObject::tgInvalid, PointF(), -1, false,id,(MapObject::EntityType)e.type, PointF(e.x,e.y)));
	}
}
void RemoteMap::setAllPlanetsOnDefense(byte team)
{
	DataContainer con = m_serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientPlanDefense);

	m_serv.SendReliable(std::move(con));
}
bool RemoteMap::filterEntityType(byte team, MapObject::EntityType et)
{
	if (Map::filterEntityType(team, et))
	{
		// anything was selected
		DataContainer con = m_serv.GetConRelSmall();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::ClientFilterEntityType);
		w.writeByte(et);

		m_serv.SendReliable(std::move(con));

		// check if any entity type is selected
		bool bEntSelect = false;

		for (const auto& e : m_ents[team - 1])
		{
			if (e.drawSelected())
			{
				if (e.getEntityType() == et)
				{
					// entities can be selected
					bEntSelect = true;
					break;
				}
			}
		}

		if (!bEntSelect)
		{
			for (const auto& p : m_plans)
			{
				if (p->getTeam() == team)
				{
					if (p->drawSelected())
					{
						if (p->getEntityType() == et)
						{
							bEntSelect = true;
							break;
						}
					}
				}
			}
		}

		if (!bEntSelect)
			return true;

		// select all ents of type et
		for (auto& e : m_ents[team - 1])
		{
			if (e.drawSelected())
			{
				if (e.getEntityType() == et)
					e.drawSelect();
				else
					e.drawDeselect();
			}
		}
		for (auto& p : m_plans)
		{
			if (p->getTeam() == team)
			{
				if (p->drawSelected())
				{
					if (p->getEntityType() == et)
						p->drawSelect();
					else
						p->drawDeselect();
				}
			}
		}

		return true;
	}
	else
		return false; // nothing was selected
}
void RemoteMap::selectAllEntityType(byte team, MapObject::EntityType et)
{
	DataContainer con = m_serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientAllEntityType);
	w.writeByte(et);

	m_serv.SendReliable(std::move(con));

	// draw select
	for (auto& e : m_ents[team - 1])
	{
		if (e.getEntityType() == et)
		{
			e.drawDeselect();
		}
		else
		{
			e.drawDeselect();
		}
	}

	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			if (p->getEntityType() == et)
				p->drawSelect();
			else
				p->drawDeselect();
		}
	}
}
RemoteMap::~RemoteMap()
{

}
bool RemoteMap::select(PointF center, float r2, byte team)
{
	//check if anything was selected
	if (Map::select(center, r2, team))
	{
		//send select message
		DataContainer c = m_serv.GetConRelSmall();
		ContainerWriter w(c);

		w.writeShort((short)PacketType::ClientSelect);
		w.writeFloat(center.x);
		w.writeFloat(center.y);
		w.writeFloat(r2);

		m_serv.SendReliable(std::move(c));

		//draw select entities
		for (auto& e : m_ents[team - 1])
		{
			if ((e.getPos() - center).lengthSq() <= r2)
			{
				e.drawSelect();
			}
			else
			{
				e.drawDeselect();
			}
		}
		for (auto& p : m_plans)
		{
			if (p->getTeam() == team)
			{
				if ((p->getPos() - center).lengthSq() <= r2)
				{
					p->drawSelect();
				}
				else
				{
					p->drawDeselect();
				}
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}
void RemoteMap::selectGroup(byte team, int group)
{
	DataContainer con = m_serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientSelectGroup);
	w.writeInt(group);

	m_serv.SendReliable(std::move(con));

	// draw select m_group
	for (auto& e : m_ents[team - 1])
	{
		if (e.getGroup() == group)
			e.drawSelect();
		else
			e.drawDeselect();
	}
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			if (p->getGroup() == group)
				p->drawSelect();
			else
				p->drawDeselect();
		}
	}
}
void RemoteMap::deleteGroup(byte team, int group)
{
	DataContainer con = m_serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientDeleteGroup);
	w.writeInt(group);

	m_serv.SendReliable(std::move(con));
}
void RemoteMap::addToGroup(byte team, int group)
{
	DataContainer con = m_serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientAddToGroup);
	w.writeInt(group);

	m_serv.SendReliable(std::move(con));
}
void RemoteMap::makeGroup(byte team, int group)
{
	DataContainer con = m_serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientMakeGroup);
	w.writeInt(group);

	m_serv.SendReliable(std::move(con));
}
void RemoteMap::selectAll(byte team)
{
	DataContainer con = m_serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientSelectAll);

	m_serv.SendReliable(std::move(con));

	// draw select all
	for (auto& e : m_ents[team - 1])
	{
		e.drawSelect();
	}
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
			p->drawSelect();
	}
}
bool RemoteMap::setTarget(PointF pt, byte team)
{
	DataContainer c = m_serv.GetConRelSmall();
	ContainerWriter w(c);

	w.writeShort((short)PacketType::ClientClick);
	w.writeFloat(pt.x);
	w.writeFloat(pt.y);

	m_serv.SendReliable(std::move(c));

	// check if planet was selected
	const MapPlanet* obj = getColPlan(pt);
	if (obj == nullptr)
		return false; // nothing 

	// a planet was selected (check if something was pre selected)

	for (auto& e : m_ents[team - 1])
	{
		if (e.drawSelected())
		{
			return true;
		}

	}

	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			if (p->drawSelected())
			{
				return true;
			}
		}
	}

	return false;
}
void RemoteMap::deselectTarget(byte team)
{
	DataContainer c = m_serv.GetConRelSmall();
	ContainerWriter w(c);

	w.writeShort((short)PacketType::ClientDeselect);
	m_serv.SendReliable(std::move(c));

	for (auto& e : m_ents[team - 1])
	{
		e.drawDeselect();
	}
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			p->drawDeselect();
		}
	}

}
void RemoteMap::update(float dt)
{
	if (m_bGameStart)
	{
		Map::resetPlanDef();
		refreshGrid();

		dt = m_clock.makeLapSecond();
		updateMovement(dt);

		for (auto& p : m_plans)
		{
			p->update(dt);
		}

		bool updPos;
		clearUpdateTime();

		m_muEnts.Lock(); //lock because of weird jumps
		receiveData(updPos); //death and movement
		if (updPos)
		{
			refreshGrid(); // because of new entities
			updateMovementServer();
		}

		m_muEnts.Unlock();
	}
	else
	{
		if (getGameTime() >= 0.0f)
			m_bGameStart = true;
	}
}
byte RemoteMap::gameEnd() const
{
	return m_winner;
}
void RemoteMap::receiveData(bool& updatePositions)
{
	updatePositions = false;

	for (const auto& p : m_dataList)
	{
		try
		{
			ContainerReader r(p);
			PacketType t = (PacketType)r.readShort();

			switch (t)
			{
			case PacketType::GameClock:
				handleSynch(r);
				break;
			case PacketType::GameSpawn:
				handleSpawn(r);
				break;
			case PacketType::GamePositions:
				handleEntPosUpdate(r);
				updatePositions = true;
				break;
			case PacketType::GameSelect:
				handleGameSelect(r);
				break;
			case PacketType::GameClick:
				handleGameClick(r);
				break;
			case PacketType::GameDeselect:
				Map::deselectTarget(r.readByte());
				break;
			case PacketType::GamePlanetCapture:
				handlePlanCap(r);
				break;
			case PacketType::GameDeath:
				handleDie(r);
				break;
			case PacketType::GamePlanetHealth:
				handlePlanUpd(r);
				break;
			case PacketType::GameEnd:
				handleGameEnd(r);
				break;

			case PacketType::GameSelectGroup:
			{
				byte team = r.readByte();
				int group = r.readInt();
				Map::selectGroup(team, group);
			}
			break;
			case PacketType::GameAddToGroup:
			{
				byte team = r.readByte();
				int group = r.readInt();
				Map::addToGroup(team, group);
			}
			break;
			case PacketType::GameDeleteGroup:
			{
				byte team = r.readByte();
				int group = r.readInt();
				Map::deleteGroup(team, group);
			}
			break;
			case PacketType::GameMakeGroup:
			{
				byte team = r.readByte();
				int group = r.readInt();
				Map::makeGroup(team, group);
			}
			break;
			case PacketType::GameSelectAll:
				Map::selectAll(r.readByte());
				break;
			case PacketType::GameTeamSurrender:
				handleSurrender(r);
				break;
			case PacketType::GameClanChange:
				handleClanChange(r);
				break;
			case PacketType::GameSetPlanetSpawntpye:
				handlePlanetSpawntype(r);
				break;
			case PacketType::GameAllEntityType:
				handleSelectAllEntityType(r);
				break;
			case PacketType::GameFilterEntityType:
				handleFilterEntityType(r);
				break;
			case PacketType::ServerPlanDefense:
				handlePlanDefend(r);
				break;
			}
		}
		catch (const std::logic_error& e)
		{
			Log::Warning(std::string("invalid package: ") + e.what());
		}
	}
	m_dataList.clear();
}
void RemoteMap::handleSpawn(ContainerReader& r)
{
	byte curTeam = r.readByte();
	PointF pos;

	// entities are locked while receive data
	while (curTeam != 0)
	{
		pos.x = r.readFloat();
		pos.y = r.readFloat();
		PlanetID pID = r.readByte();
		assert((unsigned)pID < m_nPlans);
		if ((unsigned)pID >= m_nPlans)
			throw std::domain_error("HandleSpawn");


		m_plans[pID]->resetSpawnTime();


		unsigned int id = m_ents[curTeam - 1].lastID() + 1;
		MapObject::EntityType type = (MapObject::EntityType)r.readByte();
		PointF planPos = m_plans[pID]->getPos();

		m_ents[curTeam - 1].add(new RemoteEntity(pos, curTeam, m_plans[pID]->getTargetType(), m_plans[pID]->getTarget(),
			m_plans[pID]->getGroup(), m_plans[pID]->selected(), id,type,planPos));
		Map::Event_EntitySpawn(pID, *m_ents[curTeam - 1].back());

		curTeam = r.readByte();
	}
}
void RemoteMap::handleGameSelect(ContainerReader& r)
{
	// TODO check m_team boundries
	byte team = r.readByte();
	PointF center;
	center.x = r.readFloat();
	center.y = r.readFloat();
	const float r2 = r.readFloat();
	const int n = r.readInt();
	if (n != m_ents[team - 1].length())
		throw std::domain_error("HandleGameSelect");
	assert(n == m_ents[team - 1].length());
	if (n == 0) return;

	byte myByte = r.readByte();
	const byte Mask = 0x80; //0b10000000;
	byte in = 0;

	//update ents
	for (auto& e : m_ents[team - 1])
	{
		if (in == 8)
		{
			in = 0;
			myByte = r.readByte();
		}

		if (myByte & Mask)
		{
			e.forceSelect();
		}
		else
		{
			e.deselect();
		}
		++in;
		myByte = myByte << 1;
	}

	//update planets
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			float pr2 = (p->getPos() - center).lengthSq();

			if (pr2 <= r2)
			{
				p->forceSelect();
			}
			else
			{
				p->deselect();
			}
		}
	}
}
void RemoteMap::handleGameClick(ContainerReader& r)
{
	const byte team = r.readByte();
	PointF pt;
	pt.x = r.readFloat();
	pt.y = r.readFloat();
	MapEntity::TargetType t = (MapEntity::TargetType)r.readByte();

	bool uSelected = false;

	for (auto& e : m_ents[team - 1])
	{
		if (e.selected())
		{
			e.setTarget(pt, t);
			uSelected = true;
		}
	}

	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			if (p->selected())
			{
				p->setTarget(pt, t);
				uSelected = true;
			}
		}
	}

	if (!uSelected)
	{
		if (t == MapObject::tgPlanetDefend)
		{
			// select planet
			PlanetID pid = (PlanetID)pt.x;
			m_plans[pid]->forceSelect();
		}
	}
}
void RemoteMap::handleEntPosUpdate(ContainerReader& r)
{
	float dt = m_clock.getTimeDelta(r.readFloat()); // clientTime - serverTime
	if (dt < 0.004f) dt = 0.0f; // 4ms

	const byte team = r.readByte();

	unsigned int id = r.readInt();

	PointF pos;
	bool mood;
	auto e = m_ents[team - 1].begin();
	const auto end = m_ents[0].end();

	if (e == end)
		return;

	do
	{
		while (e.GetID() < id)
		{
			++e;
			if (e == end)
				return;
		}
		pos.x = r.readFloat();
		pos.y = r.readFloat();
		mood = r.readByte() != 0;

		assert(e != end);
		if (e.GetID() == id)
		{
			e->setPosition(pos);
			if (e->getMood() != mood)
				e->changeMood();

			e->setUpdateTime(dt);
		}

		id = r.readInt();
	} while (id != 0);
	/*
	float dt = gameTime.GetTimeDelta(r.readFloat());
	D3DCOLOR colo = C_RED;
	dt = max(dt, 0.0f);
	if (dt == 0.0f)
		colo = C_DARKGREEN;

	const byte m_team = r.readByte();

	int id = r.readInt();
	PointF pos;
	bool mood;
	auto e = ents[m_team - 1].begin();
	const auto end = ents[0].end();

	if (e == end)
		return;
	do
	{
		while (e.GetID() < id)
		{
			++e;
			if (e == end)
				return;
		}
		pos.x = r.readFloat();
		pos.y = r.readFloat();
		mood = r.readByte();

		assert(e != end);
		if (e.GetID() == id)
		{
			e->SetPosition(pos);
			if (e->GetMood() != mood)
				e->ChangeMood();

			e->SetUpdateTime(dt);
		}

		id = r.readInt();
	} while (id != 0);
	*/
}
void RemoteMap::updateMovement(float dt)
{
	const float ds = dt * float(MapEntity::SPEED);

	std::function<void(EntIterator, EntIterator)> updFunc = [this, ds, dt](EntIterator start, EntIterator end)
	{
		if (start == end)
			return;

		byte team = start->getTeam();

		for (auto i = start; i != end; ++i)
		{
			setPrimaryEntVel(ds, (*i));
		}

		for (auto i = start; i != end; ++i)
		{
			if (i->getVelCorrect())
				setCrowdEntVel(ds, (*i), i.GetID());
		}

		for (auto i = start; i != end; ++i)
		{
			setEntPositionRemote((*i), dt);
		}
	};

	updateEntsWithUpdater(updFunc);
}
void RemoteMap::handleDie(ContainerReader& r)
{
	// death events are sent in order from m_team 1 to n
	byte curTeam = r.readByte();
	byte lastTeam = curTeam;
	if (curTeam <= 0)
		return;

	EntIterator curEnt = m_ents[curTeam - 1].begin();

	while (curTeam != 0)
	{
		if (curTeam != lastTeam)
		{
			lastTeam = curTeam;
			curEnt = m_ents[curTeam - 1].begin();
		}

		// search ent
		unsigned int id = r.readInt();
		while (curEnt.GetID() < id)
		{
			++curEnt;
			assert(curEnt != m_ents[curTeam - 1].end());
		}

		if (curEnt.GetID() == id)
		{
			// die
			Map::Event_EntityKilled(*curEnt);
			curEnt.remove();
		}

		curTeam = r.readByte();
	}
}
void RemoteMap::handlePlanCap(ContainerReader& r)
{
	PlanetID pID = r.readByte();
	byte newT = r.readByte();
	byte oldT = m_plans[pID]->getTeam();
	byte hasCulprit = r.readByte();

	byte cTeam = 0;
	size_t cID = 0;
	if (hasCulprit)
	{
		cTeam = r.readByte();
		cID = r.readInt();

		if (unsigned(cTeam - 1) >= m_nPlayers)
			hasCulprit = false;
	}

	m_plans[pID]->takeOver(newT);

	if (hasCulprit)
	{
		auto cul = m_ents[cTeam - 1].search(cID);
		Map::Event_PlanetCaptured(pID, newT, oldT, &(*cul));
	}
	else
	{
		Map::Event_PlanetCaptured(pID, newT, oldT, nullptr);
	}
}
void RemoteMap::handlePlanUpd(ContainerReader& r)
{
	byte start = r.readByte();
	while (r.remaining() >= 6 && start < m_nPlans)
	{
		byte team = r.readByte();
		byte subteam = r.readByte();
		int hp = r.readInt();
		if (m_plans[start]->getTeam() == team)
		{
			m_plans[start]->setSubAndHP(subteam, hp);
		}
		start++;
	}
}
void RemoteMap::handleGameEnd(ContainerReader& r)
{
	m_winner = r.readByte();
}
void RemoteMap::handleSynch(ContainerReader& r)
{
	m_clock.synchClock(r.readFloat(), 0.0f); //serv.GetPing() / 4.0f);
}

void RemoteMap::clearUpdateTime()
{
	for (size_t i = 0; i < m_nPlayers; ++i)
	{
		for (auto& e : m_ents[i])
		{
			e.setUpdateTime(0.0f);
		}
	}
}
void RemoteMap::updateMovementServer()
{
	std::function<void(EntIterator, EntIterator)> updFunc = [this](EntIterator start, EntIterator end)
	{
		if (start == end)
			return;

		byte team = start->getTeam();

		for (auto i = start; i != end; ++i)
		{
			if (i->getUpdateTime() != 0.0f)
				setPrimaryEntVel(i->getUpdateTime() * float(MapEntity::SPEED), (*i));
		}

		for (auto i = start; i != end; ++i)
		{
			if (i->getUpdateTime() != 0.0f)
				if (i->getVelCorrect())
					setCrowdEntVel(i->getUpdateTime() * float(MapEntity::SPEED), (*i), i.GetID());
		}

		for (auto i = start; i != end; ++i)
		{
			if (i->getUpdateTime() != 0.0f)
				setEntPositionRemote((*i), i->getUpdateTime());
		}
	};

	updateEntsWithUpdater(updFunc);

	/*
	for (int index = 0; index < nPlayers; index++)
	{
		for (auto i = ents[index].begin(), end = ents[index].end(); i != end; i++)
		{
			if (i->GetUpdateTime() != 0.0f)
				if (i->getVelCorrect())
					SetCrowdEntVel(i->GetUpdateTime() * float(MapEntity::SPEED), (*i), i.GetID());
		}
	}
	*/

}
void RemoteMap::handleSurrender(ContainerReader& r)
{
	byte team = r.readByte();

	// kill entities
	while (m_ents[team - 1].length() != 0)
	{
		Event_EntityKilled(*m_ents[team - 1].begin());
		m_ents[team - 1].begin().remove();
	}

	// free planets
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			p->takeOver(0);
			Event_PlanetCaptured(p->getID(), 0, team, nullptr);
		}
	}
}
void RemoteMap::handleClanChange(ContainerReader& r)
{
	byte t1 = r.readByte();
	ClanInfo i1 = ClanInfo(r.readByte());

	byte t2 = r.readByte();
	ClanInfo i2 = ClanInfo(r.readByte());

	assert(t1 >= 0 && t1 <= m_nPlayers);
	assert(t2 >= 0 && t2 <= m_nPlayers);

	m_clanInfo[t1 - 1][t2 - 1] = i1;
	m_clanInfo[t2 - 1][t1 - 1] = i2;
}

void RemoteMap::setPlanetSpawnType(byte team, MapObject::EntityType t)
{
	DataContainer con = m_serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientSetPlanetSpawntype);
	w.writeByte(t);

	m_serv.SendReliable(std::move(con));
}

void RemoteMap::handlePlanetSpawntype(ContainerReader& r)
{
	MapObject::EntityType et = (MapObject::EntityType)r.readByte();
	byte team = r.readByte();

	if (et > MapObject::etSaboteur)
		return;

	Map::setPlanetSpawnType(team, et);
}

void RemoteMap::handleSelectAllEntityType(ContainerReader& r)
{
	if (r.remaining() != 2)
		return;

	byte team = r.readByte();
	MapObject::EntityType et = (MapObject::EntityType)r.readByte();

	if (unsigned(team - 1) >= m_nPlayers)
		return;

	if (et >= MapObject::etNone)
		return;

	Map::selectAllEntityType(team, et);
}
void RemoteMap::handleFilterEntityType(ContainerReader& r)
{
	byte team = r.readByte();
	MapObject::EntityType et = (MapObject::EntityType)r.readByte();

	if (unsigned(team - 1) >= m_nPlayers)
		return;

	if (et >= MapObject::etNone)
		return;

	// target ent is selected!
	for (auto& e : m_ents[team - 1])
	{
		if (e.selected())
		{
			if (e.getEntityType() == et)
				e.forceSelect();
			else
				e.deselect();
		}
	}
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			if (p->selected())
			{
				if (p->getEntityType() == et)
					p->forceSelect();
				else
					p->deselect();
			}
		}
	}
}

void RemoteMap::addPacket(DataContainer&& con)
{
	m_dataList.push_back(std::move(con));
}

void RemoteMap::handlePlanDefend(ContainerReader& r)
{
	if (r.remaining() == 1)
	{
		byte team = r.readByte();

		Map::setAllPlanetsOnDefense(team);
	}
}