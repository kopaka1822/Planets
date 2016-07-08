#include "RemoteMap.h"
#include "PacketTypes.h"
#include "RemoteEntity.h"
#include "RemotePlanet.h"

RemoteMap::RemoteMap(Client& cl, int nPlay, const std::vector< MapLoader::MapPlanet >& planets, const std::vector< MapLoader::MapEntity >& entities,
	float width, float height, float gameTime, Map::GameType ty, std::vector< byte > clns)
	:
	Map(nPlay, planets.size(),width,height,ty,clns),
	serv(cl), clock(gameTime)
{
	//Load planets
	for (unsigned int i = 0; i < planets.size(); ++i)
	{
		plans.push_back(new RemotePlanet(planets[i], i));
	}

	//set entities
	for (const auto& e : entities)
	{
		unsigned int id = ents[e.team - 1].lastID() + 1;
		ents[e.team - 1].add(new RemoteEntity(PointF{ e.x, e.y }, e.team, MapObject::tgInvalid, PointF(), -1, false,id,(MapObject::entityType)e.type, PointF(e.x,e.y)));
	}
}
void RemoteMap::SetAllPlanetsOnDefense(byte team)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientPlanDefense);

	serv.SendReliable(std::move(con));
}
bool RemoteMap::FilterEntityType(byte team, MapObject::entityType et)
{
	if (Map::FilterEntityType(team, et))
	{
		// anything was selected
		DataContainer con = serv.GetConRelSmall();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::ClientFilterEntityType);
		w.writeByte(et);

		serv.SendReliable(std::move(con));

		// check if any entity type is selected
		bool bEntSelect = false;

		for (const auto& e : ents[team - 1])
		{
			if (e.drawSelected())
			{
				if (e.GetEntityType() == et)
				{
					// entities can be selected
					bEntSelect = true;
					break;
				}
			}
		}

		if (!bEntSelect)
		{
			for (const auto& p : plans)
			{
				if (p->GetTeam() == team)
				{
					if (p->drawSelected())
					{
						if (p->GetEntityType() == et)
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
		for (auto& e : ents[team - 1])
		{
			if (e.drawSelected())
			{
				if (e.GetEntityType() == et)
					e.drawSelect();
				else
					e.drawDeselect();
			}
		}
		for (auto& p : plans)
		{
			if (p->GetTeam() == team)
			{
				if (p->drawSelected())
				{
					if (p->GetEntityType() == et)
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
void RemoteMap::SelectAllEntityType(byte team, MapObject::entityType et)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientAllEntityType);
	w.writeByte(et);

	serv.SendReliable(std::move(con));

	// draw select
	for (auto& e : ents[team - 1])
	{
		if (e.GetEntityType() == et)
		{
			e.drawDeselect();
		}
		else
		{
			e.drawDeselect();
		}
	}

	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
		{
			if (p->GetEntityType() == et)
				p->drawSelect();
			else
				p->drawDeselect();
		}
	}
}
RemoteMap::~RemoteMap()
{

}
bool RemoteMap::Select(PointF center, float r2, byte team)
{
	//check if anything was selected
	if (Map::Select(center, r2, team))
	{
		//send select message
		DataContainer c = serv.GetConRelSmall();
		ContainerWriter w(c);

		w.writeShort((short)PacketType::ClientSelect);
		w.writeFloat(center.x);
		w.writeFloat(center.y);
		w.writeFloat(r2);

		serv.SendReliable(std::move(c));

		//draw select entities
		for (auto& e : ents[team - 1])
		{
			if ((e.GetPos() - center).lengthSq() <= r2)
			{
				e.drawSelect();
			}
			else
			{
				e.drawDeselect();
			}
		}
		for (auto& p : plans)
		{
			if (p->GetTeam() == team)
			{
				if ((p->GetPos() - center).lengthSq() <= r2)
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
void RemoteMap::SelectGroup(byte team, int group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientSelectGroup);
	w.writeInt(group);

	serv.SendReliable(std::move(con));

	// draw select group
	for (auto& e : ents[team - 1])
	{
		if (e.GetGroup() == group)
			e.drawSelect();
		else
			e.drawDeselect();
	}
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
		{
			if (p->GetGroup() == group)
				p->drawSelect();
			else
				p->drawDeselect();
		}
	}
}
void RemoteMap::DeleteGroup(byte team, int group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientDeleteGroup);
	w.writeInt(group);

	serv.SendReliable(std::move(con));
}
void RemoteMap::AddToGroup(byte team, int group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientAddToGroup);
	w.writeInt(group);

	serv.SendReliable(std::move(con));
}
void RemoteMap::MakeGroup(byte team, int group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientMakeGroup);
	w.writeInt(group);

	serv.SendReliable(std::move(con));
}
void RemoteMap::SelectAll(byte team)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientSelectAll);

	serv.SendReliable(std::move(con));

	// draw select all
	for (auto& e : ents[team - 1])
	{
		e.drawSelect();
	}
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
			p->drawSelect();
	}
}
bool RemoteMap::Click(PointF pt, byte team)
{
	DataContainer c = serv.GetConRelSmall();
	ContainerWriter w(c);

	w.writeShort((short)PacketType::ClientClick);
	w.writeFloat(pt.x);
	w.writeFloat(pt.y);

	serv.SendReliable(std::move(c));

	// check if planet was selected
	const MapPlanet* obj = GetColPlan(pt);
	if (obj == nullptr)
		return false; // nothing 

	// a planet was selected (check if something was pre selected)

	for (auto& e : ents[team - 1])
	{
		if (e.drawSelected())
		{
			return true;
		}

	}

	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
		{
			if (p->drawSelected())
			{
				return true;
			}
		}
	}

	return false;
}
void RemoteMap::ClickRight(byte team)
{
	DataContainer c = serv.GetConRelSmall();
	ContainerWriter w(c);

	w.writeShort((short)PacketType::ClientDeselect);
	serv.SendReliable(std::move(c));

	for (auto& e : ents[team - 1])
	{
		e.drawDeselect();
	}
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
		{
			p->drawDeselect();
		}
	}

}
void RemoteMap::Update(float dt)
{
	if (bGameStart)
	{
		Map::ResetPlanDef();
		RefreshGrid();

		dt = clock.MakeLapSecond();
		UpdateMovement(dt);

		for (auto& p : plans)
		{
			p->Update(dt);
		}

		bool updPos;
		ClearUpdateTime();

		muEnts.Lock(); //lock because of weird jumps
		ReceiveData(updPos); //death and movement
		if (updPos)
		{
			RefreshGrid(); // because of new entities
			UpdateMovementServer();
		}

		muEnts.Unlock();
	}
	else
	{
		if (GetGameTime() >= 0.0f)
			bGameStart = true;
	}
}
byte RemoteMap::GameEnd() const
{
	return winner;
}
void RemoteMap::ReceiveData(bool& updatePositions)
{
	updatePositions = false;

	for (const auto& p : dataList)
	{
		try
		{
			ContainerReader r(p);
			PacketType t = (PacketType)r.readShort();

			switch (t)
			{
			case PacketType::GameClock:
				HandleSynch(r);
				break;
			case PacketType::GameSpawn:
				HandleSpawn(r);
				break;
			case PacketType::GamePositions:
				HandleEntPosUpdate(r);
				updatePositions = true;
				break;
			case PacketType::GameSelect:
				HandleGameSelect(r);
				break;
			case PacketType::GameClick:
				HandleGameClick(r);
				break;
			case PacketType::GameDeselect:
				Map::ClickRight(r.readByte());
				break;
			case PacketType::GamePlanetCapture:
				HandlePlanCap(r);
				break;
			case PacketType::GameDeath:
				HandleDie(r);
				break;
			case PacketType::GamePlanetHealth:
				HandlePlanUpd(r);
				break;
			case PacketType::GameEnd:
				HandleGameEnd(r);
				break;

			case PacketType::GameSelectGroup:
			{
				byte team = r.readByte();
				int group = r.readInt();
				Map::SelectGroup(team, group);
			}
			break;
			case PacketType::GameAddToGroup:
			{
				byte team = r.readByte();
				int group = r.readInt();
				Map::AddToGroup(team, group);
			}
			break;
			case PacketType::GameDeleteGroup:
			{
				byte team = r.readByte();
				int group = r.readInt();
				Map::DeleteGroup(team, group);
			}
			break;
			case PacketType::GameMakeGroup:
			{
				byte team = r.readByte();
				int group = r.readInt();
				Map::MakeGroup(team, group);
			}
			break;
			case PacketType::GameSelectAll:
				Map::SelectAll(r.readByte());
				break;
			case PacketType::GameTeamSurrender:
				HandleSurrender(r);
				break;
			case PacketType::GameClanChange:
				HandleClanChange(r);
				break;
			case PacketType::GameSetPlanetSpawntpye:
				HandlePlanetSpawntype(r);
				break;
			case PacketType::GameAllEntityType:
				HandleSelectAllEntityType(r);
				break;
			case PacketType::GameFilterEntityType:
				HandleFilterEntityType(r);
				break;
			case PacketType::ServerPlanDefense:
				HandlePlanDefend(r);
				break;
			}
		}
		catch (const std::logic_error& e)
		{
			Log::Warning(std::string("invalid package: ") + e.what());
		}
	}
	dataList.clear();
}
void RemoteMap::HandleSpawn(ContainerReader& r)
{
	byte curTeam = r.readByte();
	PointF pos;

	// entities are locked while receive data
	while (curTeam != 0)
	{
		pos.x = r.readFloat();
		pos.y = r.readFloat();
		PlanetID pID = r.readByte();
		assert((unsigned)pID < nPlans);
		if ((unsigned)pID >= nPlans)
			throw std::domain_error("HandleSpawn");


		plans[pID]->ResetSpawnTime();


		unsigned int id = ents[curTeam - 1].lastID() + 1;
		MapObject::entityType type = (MapObject::entityType)r.readByte();
		PointF planPos = plans[pID]->GetPos();

		ents[curTeam - 1].add(new RemoteEntity(pos, curTeam, plans[pID]->GetTargetType(), plans[pID]->GetTarget(),
			plans[pID]->GetGroup(), plans[pID]->selected(), id,type,planPos));
		Map::Event_EntitySpawn(pID, *ents[curTeam - 1].back());

		curTeam = r.readByte();
	}
}
void RemoteMap::HandleGameSelect(ContainerReader& r)
{
	// TODO check team boundries
	byte team = r.readByte();
	PointF center;
	center.x = r.readFloat();
	center.y = r.readFloat();
	const float r2 = r.readFloat();
	const int n = r.readInt();
	if (n != ents[team - 1].length())
		throw std::domain_error("HandleGameSelect");
	assert(n == ents[team - 1].length());
	if (n == 0) return;

	byte myByte = r.readByte();
	const byte Mask = 0x80; //0b10000000;
	byte in = 0;

	//update ents
	for (auto& e : ents[team - 1])
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
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
		{
			float pr2 = (p->GetPos() - center).lengthSq();

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
void RemoteMap::HandleGameClick(ContainerReader& r)
{
	const byte team = r.readByte();
	PointF pt;
	pt.x = r.readFloat();
	pt.y = r.readFloat();
	MapEntity::targetType t = (MapEntity::targetType)r.readByte();

	bool uSelected = false;

	for (auto& e : ents[team - 1])
	{
		if (e.selected())
		{
			e.setTarget(pt, t);
			uSelected = true;
		}
	}

	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
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
			plans[pid]->forceSelect();
		}
	}
}
void RemoteMap::HandleEntPosUpdate(ContainerReader& r)
{
	float dt = clock.GetTimeDelta(r.readFloat()); // clientTime - serverTime
	if (dt < 0.004f) dt = 0.0f; // 4ms

	const byte team = r.readByte();

	unsigned int id = r.readInt();

	PointF pos;
	bool mood;
	auto e = ents[team - 1].begin();
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
		mood = r.readByte() != 0;

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
	/*
	float dt = gameTime.GetTimeDelta(r.readFloat());
	D3DCOLOR colo = C_RED;
	dt = max(dt, 0.0f);
	if (dt == 0.0f)
		colo = C_DARKGREEN;

	const byte team = r.readByte();

	int id = r.readInt();
	PointF pos;
	bool mood;
	auto e = ents[team - 1].begin();
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
void RemoteMap::UpdateMovement(float dt)
{
	const float ds = dt * float(MapEntity::SPEED);

	std::function<void(EntIterator, EntIterator)> updFunc = [this, ds, dt](EntIterator start, EntIterator end)
	{
		if (start == end)
			return;

		byte team = start->GetTeam();

		for (auto i = start; i != end; ++i)
		{
			SetPrimaryEntVel(ds, (*i));
		}

		for (auto i = start; i != end; ++i)
		{
			if (i->getVelCorrect())
				SetCrowdEntVel(ds, (*i), i.GetID());
		}

		for (auto i = start; i != end; ++i)
		{
			SetEntPositionRemote((*i), dt);
		}
	};

	UpdateEntsWithUpdater(updFunc);
}
void RemoteMap::HandleDie(ContainerReader& r)
{
	// death events are sent in order from team 1 to n
	byte curTeam = r.readByte();
	byte lastTeam = curTeam;
	if (curTeam <= 0)
		return;

	EntIterator curEnt = ents[curTeam - 1].begin();

	while (curTeam != 0)
	{
		if (curTeam != lastTeam)
		{
			lastTeam = curTeam;
			curEnt = ents[curTeam - 1].begin();
		}

		// search ent
		unsigned int id = r.readInt();
		while (curEnt.GetID() < id)
		{
			++curEnt;
			assert(curEnt != ents[curTeam - 1].end());
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
void RemoteMap::HandlePlanCap(ContainerReader& r)
{
	PlanetID pID = r.readByte();
	byte newT = r.readByte();
	byte oldT = plans[pID]->GetTeam();
	byte hasCulprit = r.readByte();

	byte cTeam = 0;
	size_t cID = 0;
	if (hasCulprit)
	{
		cTeam = r.readByte();
		cID = r.readInt();

		if (unsigned(cTeam - 1) >= nPlayers)
			hasCulprit = false;
	}

	plans[pID]->TakeOver(newT);

	if (hasCulprit)
	{
		auto cul = ents[cTeam - 1].search(cID);
		Map::Event_PlanetCaptured(pID, newT, oldT, &(*cul));
	}
	else
	{
		Map::Event_PlanetCaptured(pID, newT, oldT, nullptr);
	}
}
void RemoteMap::HandlePlanUpd(ContainerReader& r)
{
	byte start = r.readByte();
	while (r.remaining() >= 6 && start < nPlans)
	{
		byte team = r.readByte();
		byte subteam = r.readByte();
		int hp = r.readInt();
		if (plans[start]->GetTeam() == team)
		{
			plans[start]->SetSubAndHP(subteam, hp);
		}
		start++;
	}
}
void RemoteMap::HandleGameEnd(ContainerReader& r)
{
	winner = r.readByte();
}
void RemoteMap::HandleSynch(ContainerReader& r)
{
	clock.SynchClock(r.readFloat(), 0.0f); //serv.GetPing() / 4.0f);
}

void RemoteMap::ClearUpdateTime()
{
	for (size_t i = 0; i < nPlayers; ++i)
	{
		for (auto& e : ents[i])
		{
			e.SetUpdateTime(0.0f);
		}
	}
}
void RemoteMap::UpdateMovementServer()
{
	std::function<void(EntIterator, EntIterator)> updFunc = [this](EntIterator start, EntIterator end)
	{
		if (start == end)
			return;

		byte team = start->GetTeam();

		for (auto i = start; i != end; ++i)
		{
			if (i->GetUpdateTime() != 0.0f)
				SetPrimaryEntVel(i->GetUpdateTime() * float(MapEntity::SPEED), (*i));
		}

		for (auto i = start; i != end; ++i)
		{
			if (i->GetUpdateTime() != 0.0f)
				if (i->getVelCorrect())
					SetCrowdEntVel(i->GetUpdateTime() * float(MapEntity::SPEED), (*i), i.GetID());
		}

		for (auto i = start; i != end; ++i)
		{
			if (i->GetUpdateTime() != 0.0f)
				SetEntPositionRemote((*i), i->GetUpdateTime());
		}
	};

	UpdateEntsWithUpdater(updFunc);

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
void RemoteMap::HandleSurrender(ContainerReader& r)
{
	byte team = r.readByte();

	// kill entities
	while (ents[team - 1].length() != 0)
	{
		Event_EntityKilled(*ents[team - 1].begin());
		ents[team - 1].begin().remove();
	}

	// free planets
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
		{
			p->TakeOver(0);
			Event_PlanetCaptured(p->GetID(), 0, team, nullptr);
		}
	}
}
void RemoteMap::HandleClanChange(ContainerReader& r)
{
	byte t1 = r.readByte();
	ClanInfo i1 = ClanInfo(r.readByte());

	byte t2 = r.readByte();
	ClanInfo i2 = ClanInfo(r.readByte());

	assert(t1 >= 0 && t1 <= nPlayers);
	assert(t2 >= 0 && t2 <= nPlayers);

	clanInfo[t1 - 1][t2 - 1] = i1;
	clanInfo[t2 - 1][t1 - 1] = i2;
}

void RemoteMap::SetPlanetSpawnType(byte team, MapObject::entityType t)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ClientSetPlanetSpawntype);
	w.writeByte(t);

	serv.SendReliable(std::move(con));
}

void RemoteMap::HandlePlanetSpawntype(ContainerReader& r)
{
	MapObject::entityType et = (MapObject::entityType)r.readByte();
	byte team = r.readByte();

	if (et > MapObject::etSaboteur)
		return;

	Map::SetPlanetSpawnType(team, et);
}

void RemoteMap::HandleSelectAllEntityType(ContainerReader& r)
{
	if (r.remaining() != 2)
		return;

	byte team = r.readByte();
	MapObject::entityType et = (MapObject::entityType)r.readByte();

	if (unsigned(team - 1) >= nPlayers)
		return;

	if (et >= MapObject::etNone)
		return;

	Map::SelectAllEntityType(team, et);
}
void RemoteMap::HandleFilterEntityType(ContainerReader& r)
{
	byte team = r.readByte();
	MapObject::entityType et = (MapObject::entityType)r.readByte();

	if (unsigned(team - 1) >= nPlayers)
		return;

	if (et >= MapObject::etNone)
		return;

	// target ent is selected!
	for (auto& e : ents[team - 1])
	{
		if (e.selected())
		{
			if (e.GetEntityType() == et)
				e.forceSelect();
			else
				e.deselect();
		}
	}
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
		{
			if (p->selected())
			{
				if (p->GetEntityType() == et)
					p->forceSelect();
				else
					p->deselect();
			}
		}
	}
}

void RemoteMap::AddPacket(DataContainer&& con)
{
	dataList.push_back(std::move(con));
}

void RemoteMap::HandlePlanDefend(ContainerReader& r)
{
	if (r.remaining() == 1)
	{
		byte team = r.readByte();

		Map::SetAllPlanetsOnDefense(team);
	}
}