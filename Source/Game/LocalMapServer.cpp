#include "LocalMapServer.h"
#include "../Game/PacketTypes.h"

LocalMapServer::LocalMapServer(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets,
	const std::vector<MapLoader::MapSpawn>& spawns, Server& serv, float width, float height,
	std::function<TeamID(int)> GetPlayerTeam, Map::GameType ty, std::vector< TeamID > clns)
	:
	LocalMap(nPlayers,planets.size(),width,height, ty, clns),
	serv(serv),
	GetPlayerTeam(GetPlayerTeam),
	maxEntLen(std::min((MAX_PACKSIZE - (25)) * 8,unsigned(5000))),
	MAX_PACKSIZE(NetServer::GetMaxPackSize()),
	startTeams(clns)
{
	LocalMap::loadMapComponents(planets, spawns);
}

LocalMapServer::~LocalMapServer()
{

}

std::vector< DataContainer > LocalMapServer::GetStartData()
{
	// 1st Planets
	// 2nd Entities
	// 3rd Complete
	std::vector< DataContainer > data;

	// planets
	{
		DataContainer con = serv.GetConRel();
		ContainerWriter w(con);

		w.writeShort(short(PacketType::MapPlanets));

		/*
		Message Format:
		- bool
		true : (next is planet)
		false : Packet end or no more planet
		if (true )
		float posx
		float posy
		float radius
		byte m_team
		int m_hp
		float sUnit
		*/
		int curPlanet = 0;
		while (curPlanet != m_nPlans)
		{
			w.writeChar(true);
			w.writeFloat(m_plans[curPlanet]->getPos().x);
			w.writeFloat(m_plans[curPlanet]->getPos().y);
			w.writeFloat(m_plans[curPlanet]->getRadius());
			w.writeByte(m_plans[curPlanet]->getTeam());
			w.writeInt(m_plans[curPlanet]->getMaxHP());
			w.writeFloat(m_plans[curPlanet]->getsUnit());

			curPlanet++;

		}

		// no more planets in this pack
		w.writeChar(false);

		data.push_back(std::move(con));
	}
	{
		// entities
		for (size_t curTeam = 0; curTeam < m_nPlayers; curTeam++)
		{
			if (m_ents[curTeam].length())
			{
				auto curEnt = m_ents[curTeam].begin();

				DataContainer con = serv.GetConRel();
				ContainerWriter w(con);

				w.writeShort((short)PacketType::MapEntities);
				w.writeByte((byte)(curTeam + 1));
				/*
				Message Format:
				byte m_team (once)

				- bool
				true : (next is entity)
				false : Packet end or no more planet
				if (true )
				float posx
				float posy
				byte type
				*/
				while (curEnt != m_ents[curTeam].end())
				{
					w.writeChar(true);
					w.writeFloat(curEnt->getPos().x);
					w.writeFloat(curEnt->getPos().y);
					w.writeByte((byte)curEnt->getEntityType());

					++curEnt;
				}
				w.writeChar(false); // no more enities in this pack
				data.push_back(std::move(con));
			}
		}

	}
	{
		// Teams and gametype
		DataContainer con = serv.GetConRel();
		ContainerWriter w(con);
		w.writeShort(short(PacketType::MapTeams));
		w.writeByte(byte(m_nPlayers));
		w.writeByte(byte(m_gameType));
		if (m_gameType == GameType::Allicance)
		{
			assert(startTeams.size() >= m_nPlayers);
			for (size_t i = 0; i < m_nPlayers; i++)
			{
				w.writeByte(startTeams[i]);
			}
		}

		data.push_back(std::move(con));
	}
	{
		// complete message

		DataContainer con = serv.GetConRelSmall();
		ContainerWriter w(con);

		w.writeShort((short)PacketType::MapComplete);

		// map width
		w.writeFloat(m_mWidth);
		w.writeFloat(m_mHeight);

		data.push_back(std::move(con));
	}
	return data;
}
void LocalMapServer::deselectTarget(TeamID team)
{
	DataContainer c = serv.GetConRelSmall();
	ContainerWriter w(c);

	w.writeShort(short(PacketType::GameDeselect));
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(c));
	Map::deselectTarget(team);
}
bool LocalMapServer::setTarget(PointF pt, TeamID team)
{
	//determine target type
	bool planetTargeted = false;

	MapEntity::TargetType t = MapEntity::tgPoint;
	const MapPlanet* obj = getColPlan(pt);
	if (obj != nullptr)
	{
		pt.x = obj->getID();
		planetTargeted = true;
		if (obj->getTeam() == team)
		{
			t = MapObject::tgPlanetDefend; //own planet == defend
		}
		else
		{
			t = MapObject::tgPlanet; //enemy planet = attack
		}
	}

	//send info
	DataContainer c = serv.GetConRelSmall();
	ContainerWriter w(c);
	w.writeShort(short(PacketType::GameClick));
	w.writeByte(team);
	w.writeFloat(pt.x);
	w.writeFloat(pt.y);
	w.writeByte(t);

	serv.SendContainerAllReliable(std::move(c));

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
		// nothing selected, user clicked on a planet?
		if (t == MapObject::tgPlanetDefend)
		{
			// user clicked on a planet!
			PlanetID pid = PlanetID(pt.x);
			m_plans[pid]->forceSelect();
		}
		return false;
	}
	else
	{
		return planetTargeted;
	}
}
void LocalMapServer::update(float dt)
{
	Timer t;
	t.StartWatch();
	// send updates
	lastClock += dt;
	if (lastClock > 6.0f)
	{
		lastClock = 0.0f;
		SynchClock(clock.getTimeSecond());
	}

	if (bGameStart)
	{
		ReceiveData();
		refreshGrid();

		dt = clock.makeLapSecond();

		UpdatePlanets(dt);
		Map::resetPlanDef();

		// update enities
		UpdateMovement(dt, clock.getTimeSecond());
		UpdateDeath();

		winner = LocalMap::gameEnd();

		if (winner)
		{
			SendWinnerMessage();
		}
	}
	else
	{
		if (clock.getTimeSecond() >= 0.0f)
			bGameStart = true;
	}

	runs++;
	sumTime += t.GetTimeMilli();
}
void LocalMapServer::SendWinnerMessage()
{

	DataContainer c = serv.GetConRelSmall();
	ContainerWriter w(c);

	w.writeShort(short(PacketType::GameEnd));
	w.writeByte(winner); //winner clan

	serv.SendContainerAllReliable(std::move(c));
}
void LocalMapServer::UpdateMovement(float dt, const float curTime)
{

	const float ds = dt * float(MapEntity::SPEED);

	std::function<void(EntIterator, EntIterator)> updFunc = [this, ds, dt](EntIterator start, EntIterator end)
	{
		if (start == end)
			return;

		//byte m_team = start->GetTeam();

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
			setEntPosition((*i), dt);
			AttackNearby(*i);
		}
	};

	updateEntsWithUpdater(updFunc);

	//send updates
	//position updates

	moveTime += dt;
	if (lastTeamUpdate > 0 ||  moveTime > CalcMoveResendTime())
	{
		//send position updates
		if (lastTeamUpdate == 0)
		{
			lastTeamUpdate = 1; // send pos updates for m_team 1
		}

		moveTime = 0.0f;
		//for (int index = 0; index < nPlayers; ++index)
		{
			int index = lastTeamUpdate - 1;

			auto e = m_ents[index].begin();
			const auto end = m_ents[index].end();

			if (e != end)
			{

				do
				{
					DataContainer c = serv.GetConUnrel();
					ContainerWriter w(c);


					w.writeShort(short(PacketType::GamePositions));
					w.writeFloat(curTime);
					w.writeByte(index + 1); //m_team

					do
					{
						w.writeInt(e.GetID());
						w.writeFloat(e->getPos().x);
						w.writeFloat(e->getPos().y);
						w.writeByte(e->getMood());

						++e;
					} while (e != end && c.length() < MAX_PACKSIZE);
					w.writeInt(0); //ending (byte)


					serv.SendContainerAllUnreliable(std::move(c));
				} while (e != end);
			}
			// m_team updates done
			lastTeamUpdate += 1;
			if (lastTeamUpdate > m_nPlayers)
			{
				lastTeamUpdate = 0;
			}
		}
	}

	planTime += dt;
	if (planTime > 0.3f)
	{
		planTime = 0.0f;
		// planet health
		auto p = m_plans.begin();
		const auto end = m_plans.end();
		if (p != end)
		{
			do
			{
				DataContainer c = serv.GetConUnrel();
				ContainerWriter w(c);
				w.writeShort(short(PacketType::GamePlanetHealth));
				w.writeByte(byte((*p)->getID())); //  start planet

				while (c.length() < MAX_PACKSIZE && p != end)
				{
					w.writeByte((*p)->getTeam());
					w.writeByte((*p)->getSubteam());
					w.writeInt((*p)->getHP());

					++p;
				}

				serv.SendContainerAllUnreliable(std::move(c));
			} while (p != end);
		}
	}

}
float LocalMapServer::CalcMoveResendTime() const
{
	static const int nPerPack = MAX_PACKSIZE / (13); // pos x, y, id, mood

	unsigned int nEnts = 0;
	for (size_t i = 0; i < m_nPlayers; i++)
		nEnts += m_ents[i].length();

	if (nEnts == 0)
		return 1000000.0f; // dont send updates

	unsigned int nPacks = (nEnts / nPerPack) + 1; // c.a. 300 ents in one package

	return std::max(float(nPacks) * 0.1f, 0.5f);
}
void LocalMapServer::AttackNearby(MapEntity& curEnt)
{
	if (!curEnt.hasDamage())
		return;

	if (curEnt.attacksPlanets())
	{
		MapEntity* obj = getEnemyEnt(curEnt.getPos(), curEnt.getTeam());
		if (obj != nullptr)
		{
			obj->takeDamage(curEnt.getDamage());
			return;
		}
	}

	if (curEnt.attacksPlanets())
	{
		for (auto& p : m_plans)
		{
			if (p->isNearby(curEnt.getPos()))
			{
				const byte pTeam = p->getTeam();
				if (!isAlly(curEnt.getTeam(), pTeam))
				{
					bool capture = false;
					//byte pTeam = p->GetTeam();
					if (curEnt.getEntityType() == MapObject::etSaboteur)
					{
						p->sabotage(curEnt.getTeam());
						curEnt.setHP(0);
						capture = true;
					}
					else
					{
						byte atkTeam = curEnt.getTeam();
						if (isAlly(p->getSubteam(), atkTeam))
						{
							// push ally
							atkTeam = p->getSubteam();
						}
						capture = p->takeDamage(curEnt.getDamage(), atkTeam);
					}

					if (capture)
					{
						Event_PlanetCaptured(p->getID(), p->getTeam(), pTeam, &curEnt);
					}
					else
					{
						//Attacked
					}
				}
			}
		}
	}
}
void LocalMapServer::UpdatePlanets(float dt)
{

	struct SpawnEvent
	{
		PointF pos;
		byte team;
		PlanetID plan;
		MapObject::EntityType type;
	};

	FastVector< SpawnEvent > spEvnts;

	for (auto& p : m_plans)
	{
		if (p->getTeam() != 0)
		{
			if (m_ents[p->getTeam() - 1].length() < maxEntLen)
			{
				if (p->update(dt))
				{
					if (tryEntitySpawn(p->getPos(), p->getTeam(), p->getRadius(), p->getTargetType(), p->getTarget(), p->getGroup(),
					                   p->selected(),p->getDefenseRadius(),p->getEntityType()))
					{
						SpawnEvent s;
						s.team = p->getTeam();
						s.plan = p->getID();
						s.pos = m_ents[s.team - 1].back()->getPos();
						s.type = p->getEntityType();
						spEvnts.push_back(s);

						m_grid.addEntity(&(*(m_ents[s.team - 1].back())), s.pos);
					}
				}
			}
		}
	}

	if (spEvnts.length())
	{
		DataContainer c = serv.GetConRel();
		ContainerWriter w(c);

		w.writeShort(short(PacketType::GameSpawn));

		for (const auto& se : spEvnts)
		{
			w.writeByte(se.team);
			w.writeFloat(se.pos.x);
			w.writeFloat(se.pos.y);
			w.writeByte(byte(se.plan));
			w.writeByte(se.type);
		}
		w.writeByte(0);

		serv.SendContainerAllReliable(std::move(c));
	}
}
void LocalMapServer::ReceiveData()
{
	for (const auto& c : inData)
	{
		try
		{
			ContainerReader r(c);
			const int pid = r.readInt(); // player id
			const byte team = GetPlayerTeam(pid);
			if (team < 1 || team > m_nPlayers)
				throw std::domain_error("m_team");

			PacketType t = PacketType(r.readShort());

			switch (t)
			{
			case PacketType::ClientSelect:
				HandleClientSelect(r, team);
				break;
			case PacketType::ClientClick:
			{
				PointF pt;
				pt.x = r.readFloat();
				pt.y = r.readFloat();
				LocalMapServer::setTarget(pt, team);
			}
			break;
			case PacketType::ClientDeselect:
				LocalMapServer::deselectTarget(team);
				break;
			case PacketType::ClientAddToGroup:
				LocalMapServer::addToGroup(team, r.readInt());
				break;
			case PacketType::ClientSelectGroup:
				LocalMapServer::selectGroup(team, r.readInt());
				break;
			case PacketType::ClientMakeGroup:
				LocalMapServer::makeGroup(team, r.readInt());
				break;
			case PacketType::ClientDeleteGroup:
				LocalMapServer::deleteGroup(team, r.readInt());
				break;
			case PacketType::ClientSelectAll:
				LocalMapServer::selectAll(team);
				break;
			case PacketType::ClientClanRequest:
				HandleClientClanRequest(team, r.readByte());
				break;
			case PacketType::ClientClanDestroy:
				HandleClientClanDestroy(team, r.readByte());
				break;
			case PacketType::ClientSetPlanetSpawntype:
				HandleClientSpawntype(r, team);
				break;
			case PacketType::ClientFilterEntityType:
				filterEntityType(team, MapObject::EntityType(r.readByte()));
				break;
			case PacketType::ClientAllEntityType:
				selectAllEntityType(team, MapObject::EntityType(r.readByte()));
				break;
			case PacketType::ClientPlanDefense:
				HandleClientPlanDefense(r, team);
				break;
			}
		}
		catch (const std::logic_error&)
		{
			// TODO maybe blacklist a player if he is sending (too much) invalid data
		}
	}
	inData.clear();
}
void LocalMapServer::HandleClientSelect(ContainerReader& r, TeamID team)
{
	PointF center;
	center.x = r.readFloat();
	center.y = r.readFloat();
	const float r2 = r.readFloat();

	DataContainer c = serv.GetConRel();
	ContainerWriter w(c);

	w.writeShort(short(PacketType::GameSelect));

	w.writeByte(team);

	// this will determine which planets are selected
	w.writeFloat(center.x);
	w.writeFloat(center.y);
	w.writeFloat(r2);

	// this is for each entity
	w.writeInt(m_ents[team - 1].length()); // controll

	byte myByte = 0;
	byte in = 0;

	//select entities
	for (auto& e : m_ents[team - 1])
	{
		float er2 = (e.getPos() - center).lengthSq();
		if (er2 <= r2)
		{
			e.forceSelect();
			++myByte; //+1
		}
		else
		{
			e.deselect(); //+0
		}
		++in;
		if (in == 8)
		{
			w.writeByte(myByte);
			myByte = 0;
			in = 0;
		}
		myByte = myByte << 1;
	}
	if (in != 0)
	{
		myByte = myByte << (8 - in - 1);
		w.writeByte(myByte);
	}
	//Send container
	serv.SendContainerAllReliable(std::move(c));

	//Select Planet
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
void LocalMapServer::UpdateDeath()
{
	struct DeathEvent
	{
		byte team;
		unsigned int id;
	};
	FastVector< DeathEvent > dEv;
	DeathEvent d;

	for (size_t index = 0; index < m_nPlayers; ++index)
	{
		d.team = byte(index + 1);
		for (auto i = m_ents[index].begin(), end = m_ents[index].end(); i != end; ++i)
		{
			if (i->getHP() <= 0)
			{
				d.id = i.GetID();
				const float exprad = i->getExplosionRadius();
				if (exprad > 0.0f)
				{
					killEnts(i->getPos(), exprad, i->getExplosionDamage(), i->getTeam());
				}
				i.remove();
				dEv.push_back(d);
			}
		}
	}

	if (dEv.length())
	{
		DataContainer c = serv.GetConRel();
		ContainerWriter w(c);

		w.writeShort(short(PacketType::GameDeath));

		for (const auto& d : dEv)
		{
			w.writeByte(d.team);
			w.writeInt(d.id);
		}
		w.writeByte(0);

		serv.SendContainerAllReliable(std::move(c));
	}
}
void LocalMapServer::Event_PlanetCaptured(PlanetID pID, TeamID newTeam, TeamID oldTeam, const MapEntity* culprit)
{
	DataContainer c = serv.GetConRelSmall();
	ContainerWriter w(c);
	w.writeShort(short(PacketType::GamePlanetCapture));
	w.writeByte(byte(pID));
	w.writeByte(newTeam);
	if (culprit)
	{
		w.writeByte(1);
		w.writeByte(culprit->getTeam());
		w.writeInt(culprit->getID());
	}
	else
	{
		w.writeByte(0);
	}

	serv.SendContainerAllReliable(std::move(c));


	Map::Event_PlanetCaptured(pID, newTeam, oldTeam, culprit);
}
void LocalMapServer::SynchClock(float curTime)
{
	DataContainer con = serv.GetConUnrelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameClock));
	w.writeFloat(curTime);

	serv.SendContainerAllUnreliable(std::move(con));
}

bool LocalMapServer::isAlive(TeamID team) const
{
	if (m_ents[team - 1].length() != 0)
		return true;

	//check planet
	for (const auto& p : m_plans)
	{
		if (p->getTeam() == team)
			return true;
	}
	return false;
}

void LocalMapServer::addToGroup(TeamID team, GroupID group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameAddToGroup));
	w.writeByte(team);
	w.writeInt(group);

	serv.SendContainerAllReliable(std::move(con));

	Map::addToGroup(team, group);
}
void LocalMapServer::makeGroup(TeamID team, GroupID group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameMakeGroup));
	w.writeByte(team);
	w.writeInt(group);

	serv.SendContainerAllReliable(std::move(con));

	Map::makeGroup(team, group);
}
void LocalMapServer::selectGroup(TeamID team, GroupID group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameSelectGroup));
	w.writeByte(team);
	w.writeInt(group);

	serv.SendContainerAllReliable(std::move(con));

	Map::selectGroup(team, group);
}
void LocalMapServer::deleteGroup(TeamID team, GroupID group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameDeleteGroup));
	w.writeByte(team);
	w.writeInt(group);

	serv.SendContainerAllReliable(std::move(con));

	Map::deleteGroup(team, group);
}
void LocalMapServer::selectAll(TeamID team)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameSelectAll));
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(con));

	Map::selectAll(team);
}

bool LocalMapServer::HandleSurrender(byte team)
{
	bool bValid = false;
	if (m_ents[team - 1].length() != 0)
		bValid = true;

	if (!bValid)
	{
		// cheack plantes
		for (const auto& p : m_plans)
		{
			if (p->getTeam() == team)
			{
				bValid = true;
				break;
			}
		}
	}

	if (!bValid)
		return false;

	// send message
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameTeamSurrender));
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(con));

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
	return true;
}

void LocalMapServer::HandleClientClanRequest(TeamID asking, TeamID team)
{
	if (m_gameType != GameType::UnholyAlliance)
		return;

	if (team == 0 || team > m_nPlayers)
		return;

	if (asking == team)
		return;

	if (CountAllies(asking) >= 2)
		return;

	const ClanInfo i = m_clanInfo[asking - 1][team - 1];
	if (i == ClanInfo::noAlly)
	{
		// send ally request
		SendAllyRequest(asking, team);
	}
	else if (i == ClanInfo::sendRequest)
	{
		// accept request and form teams
		if (CountAllies(team) < 2)
			SendAllyFormed(asking, team);
		else
			SendAllyDeny(team, asking);
	}
	// send request -> do nothing because already asked
	// ally -> already allied...
}
void LocalMapServer::HandleClientClanDestroy(TeamID asking, TeamID team)
{
	if (m_gameType != GameType::UnholyAlliance)
		return;

	if (team == 0 || team > m_nPlayers)
		return;

	if (asking == team)
		return;

	const ClanInfo i = m_clanInfo[asking - 1][team - 1];

	if (i == ClanInfo::Ally)
	{
		// remove teams
		SendAllyResolve(asking, team);
	}
	else if (i == ClanInfo::sendRequest)
	{
		// delete request
		SendAllyDeny(asking, team);
	}
	else if (i == ClanInfo::awaiting)
	{
		// cancel your request
		SendAllyDeny(asking, team);
	}
}

void  LocalMapServer::SendAllyRequest(TeamID from, TeamID to)
{
	m_clanInfo[from - 1][to - 1] = ClanInfo::awaiting;
	m_clanInfo[to - 1][from - 1] = ClanInfo::sendRequest;

	SendClanUpdate(from, to);
}
void  LocalMapServer::SendAllyFormed(TeamID t1, TeamID t2)
{
	m_clanInfo[t1 - 1][t2 - 1] = ClanInfo::Ally;
	m_clanInfo[t2 - 1][t1 - 1] = ClanInfo::Ally;

	SendClanUpdate(t1, t2);
}
void  LocalMapServer::SendAllyResolve(TeamID t1, TeamID t2)
{
	m_clanInfo[t1 - 1][t2 - 1] = ClanInfo::noAlly;
	m_clanInfo[t2 - 1][t1 - 1] = ClanInfo::noAlly;

	SendClanUpdate(t1, t2);
}
void  LocalMapServer::SendAllyDeny(TeamID from, TeamID to)
{
	m_clanInfo[from - 1][to - 1] = ClanInfo::noAlly;
	m_clanInfo[to - 1][from - 1] = ClanInfo::noAlly;

	SendClanUpdate(from, to);
}

void LocalMapServer::SendClanUpdate(TeamID t1, TeamID t2)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameClanChange));
	w.writeByte(t1);
	w.writeByte(byte(m_clanInfo[t1 - 1][t2 - 1]));
	w.writeByte(t2);
	w.writeByte(byte(m_clanInfo[t2 - 1][t1 - 1]));

	serv.SendContainerAllReliable(std::move(con));
}

void LocalMapServer::HandleClientSpawntype(ContainerReader& r, TeamID team)
{
	MapObject::EntityType et = MapObject::EntityType(r.readByte());
	if (et > MapObject::etSaboteur)
		throw std::domain_error("HandleClientSpawntype");

	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameSetPlanetSpawntpye));
	w.writeByte(byte(et));
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(con));

	Map::setPlanetSpawnType(team, et);
}

bool LocalMapServer::filterEntityType(TeamID team, MapObject::EntityType et)
{
	if (unsigned(et) >= MapObject::etNone)
		return false;

	// check for any ent select first

	bool bEntSelect = false;

	for (const auto& e : m_ents[team - 1])
	{
		if (e.selected())
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
				if (p->selected())
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
		return false;

	// send message

	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameFilterEntityType));
	w.writeByte(team);
	w.writeByte(et);

	serv.SendContainerAllReliable(std::move(con));

	return LocalMap::filterEntityType(team,et);
}
void LocalMapServer::selectAllEntityType(TeamID team, MapObject::EntityType et)
{
	if (unsigned(et) >= MapObject::etNone)
		return;

	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::GameAllEntityType));
	w.writeByte(team);
	w.writeByte(et);

	serv.SendContainerAllReliable(std::move(con));

	LocalMap::selectAllEntityType(team, et);
}
void LocalMapServer::HandleClientPlanDefense(ContainerReader& r, TeamID team)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort(short(PacketType::ServerPlanDefense));
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(con));

	Map::setAllPlanetsOnDefense(team);
}