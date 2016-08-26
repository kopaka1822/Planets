#include "LocalMapServer.h"
#include "../Game/PacketTypes.h"

LocalMapServer::LocalMapServer(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets,
	const std::vector<MapLoader::MapSpawn>& spawns, Server& serv, float width, float height,
	std::function<byte(int)> GetPlayerTeam, Map::GameType ty, std::vector< byte > clns)
	:
	LocalMap(nPlayers,planets.size(),width,height, ty, clns),
	serv(serv),
	maxEntLen(std::min((MAX_PACKSIZE - (25)) * 8,unsigned(5000))),
	GetPlayerTeam(GetPlayerTeam),
	startTeams(clns),
	MAX_PACKSIZE(NetServer::GetMaxPackSize())
{
	LocalMap::LoadMapComponents(planets, spawns);
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

		w.writeShort((short)PacketType::MapPlanets);

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
		while (curPlanet != nPlans)
		{
			w.writeChar(true);
			w.writeFloat(plans[curPlanet]->getPos().x);
			w.writeFloat(plans[curPlanet]->getPos().y);
			w.writeFloat(plans[curPlanet]->getRadius());
			w.writeByte(plans[curPlanet]->getTeam());
			w.writeInt(plans[curPlanet]->getMaxHP());
			w.writeFloat(plans[curPlanet]->getsUnit());

			curPlanet++;

		}

		// no more planets in this pack
		w.writeChar(false);

		data.push_back(std::move(con));
	}
	{
		// entities
		for (size_t curTeam = 0; curTeam < nPlayers; curTeam++)
		{
			if (ents[curTeam].length())
			{
				auto curEnt = ents[curTeam].begin();

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
				while (curEnt != ents[curTeam].end())
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
		w.writeShort((short)PacketType::MapTeams);
		w.writeByte(nPlayers);
		w.writeByte((byte)gameType);
		if (gameType == GameType::Allicance)
		{
			assert(startTeams.size() >= nPlayers);
			for (size_t i = 0; i < nPlayers; i++)
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
		w.writeFloat(mWidth);
		w.writeFloat(mHeight);

		data.push_back(std::move(con));
	}
	return data;
}
void LocalMapServer::ClickRight(byte team)
{
	DataContainer c = serv.GetConRelSmall();
	ContainerWriter w(c);

	w.writeShort((short)PacketType::GameDeselect);
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(c));
	Map::ClickRight(team);
}
bool LocalMapServer::Click(PointF pt, byte team)
{
	//determine target type
	bool planetTargeted = false;

	MapEntity::TargetType t = MapEntity::tgPoint;
	const MapPlanet* obj = GetColPlan(pt);
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
	w.writeShort((short)PacketType::GameClick);
	w.writeByte(team);
	w.writeFloat(pt.x);
	w.writeFloat(pt.y);
	w.writeByte(t);

	serv.SendContainerAllReliable(std::move(c));

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
			PlanetID pid = (PlanetID)pt.x;
			plans[pid]->forceSelect();
		}
		return false;
	}
	else
	{
		return planetTargeted;
	}
}
void LocalMapServer::Update(float dt)
{
	Timer t;
	t.StartWatch();
	// send updates
	lastClock += dt;
	if (lastClock > 6.0f)
	{
		lastClock = 0.0f;
		SynchClock(clock.GetTimeSecond());
	}

	if (bGameStart)
	{
		ReceiveData();
		RefreshGrid();

		dt = clock.MakeLapSecond();

		UpdatePlanets(dt);
		Map::ResetPlanDef();

		// update enities
		UpdateMovement(dt, clock.GetTimeSecond());
		UpdateDeath();

		winner = LocalMap::GameEnd();

		if (winner)
		{
			SendWinnerMessage();
		}
	}
	else
	{
		if (clock.GetTimeSecond() >= 0.0f)
			bGameStart = true;
	}

	runs++;
	sumTime += t.GetTimeMilli();
}
void LocalMapServer::SendWinnerMessage()
{

	DataContainer c = serv.GetConRelSmall();
	ContainerWriter w(c);

	w.writeShort((short)PacketType::GameEnd);
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
			SetPrimaryEntVel(ds, (*i));
		}

		for (auto i = start; i != end; ++i)
		{
			if (i->getVelCorrect())
				SetCrowdEntVel(ds, (*i), i.GetID());
		}

		for (auto i = start; i != end; ++i)
		{
			SetEntPosition((*i), dt);
			AttackNearby(*i);
		}
	};

	UpdateEntsWithUpdater(updFunc);

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

			auto e = ents[index].begin();
			const auto end = ents[index].end();

			if (e != end)
			{

				do
				{
					DataContainer c = serv.GetConUnrel();
					ContainerWriter w(c);


					w.writeShort((short)PacketType::GamePositions);
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
			if (lastTeamUpdate > nPlayers)
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
		auto p = plans.begin();
		const auto end = plans.end();
		if (p != end)
		{
			do
			{
				DataContainer c = serv.GetConUnrel();
				ContainerWriter w(c);
				w.writeShort((short)PacketType::GamePlanetHealth);
				w.writeByte((byte)(*p)->getID()); //  start planet

				while (c.length() < MAX_PACKSIZE && p != end)
				{
					w.writeByte((*p)->getTeam());
					w.writeByte((*p)->getSubteam());
					w.writeInt((*p)->getHP());

					p++;
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
	for (size_t i = 0; i < nPlayers; i++)
		nEnts += ents[i].length();

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
		MapEntity* obj = GetEnemyEnt(curEnt.getPos(), curEnt.getTeam());
		if (obj != nullptr)
		{
			obj->takeDamage(curEnt.getDamage());
			return;
		}
	}

	if (curEnt.attacksPlanets())
	{
		for (auto& p : plans)
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

	for (auto& p : plans)
	{
		if (p->getTeam() != 0)
		{
			if (ents[p->getTeam() - 1].length() < maxEntLen)
			{
				if (p->update(dt))
				{
					if (TryEntitySpawn(p->getPos(), p->getTeam(), p->getRadius(), p->getTargetType(), p->getTarget(), p->getGroup(),
						p->selected(),p->getDefenseRadius(),p->getEntityType()))
					{
						SpawnEvent s;
						s.team = p->getTeam();
						s.plan = p->getID();
						s.pos = ents[s.team - 1].back()->getPos();
						s.type = p->getEntityType();
						spEvnts.push_back(s);

						grid.AddEntity(&(*(ents[s.team - 1].back())), s.pos);
					}
				}
			}
		}
	}

	if (spEvnts.length())
	{
		DataContainer c = serv.GetConRel();
		ContainerWriter w(c);

		w.writeShort((short)PacketType::GameSpawn);

		for (const auto& se : spEvnts)
		{
			w.writeByte(se.team);
			w.writeFloat(se.pos.x);
			w.writeFloat(se.pos.y);
			w.writeByte((byte)se.plan);
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
			if (team < 1 || team > nPlayers)
				throw std::domain_error("m_team");

			PacketType t = (PacketType)r.readShort();

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
				LocalMapServer::Click(pt, team);
			}
			break;
			case PacketType::ClientDeselect:
				LocalMapServer::ClickRight(team);
				break;
			case PacketType::ClientAddToGroup:
				LocalMapServer::AddToGroup(team, r.readInt());
				break;
			case PacketType::ClientSelectGroup:
				LocalMapServer::SelectGroup(team, r.readInt());
				break;
			case PacketType::ClientMakeGroup:
				LocalMapServer::MakeGroup(team, r.readInt());
				break;
			case PacketType::ClientDeleteGroup:
				LocalMapServer::DeleteGroup(team, r.readInt());
				break;
			case PacketType::ClientSelectAll:
				LocalMapServer::SelectAll(team);
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
				FilterEntityType(team, (MapObject::EntityType)r.readByte());
				break;
			case PacketType::ClientAllEntityType:
				SelectAllEntityType(team, (MapObject::EntityType)r.readByte());
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
void LocalMapServer::HandleClientSelect(ContainerReader& r, byte team)
{
	PointF center;
	center.x = r.readFloat();
	center.y = r.readFloat();
	const float r2 = r.readFloat();

	DataContainer c = serv.GetConRel();
	ContainerWriter w(c);

	w.writeShort((short)PacketType::GameSelect);

	w.writeByte(team);

	// this will determine which planets are selected
	w.writeFloat(center.x);
	w.writeFloat(center.y);
	w.writeFloat(r2);

	// this is for each entity
	w.writeInt(ents[team - 1].length()); // controll

	byte myByte = 0;
	byte in = 0;

	//select entities
	for (auto& e : ents[team - 1])
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
	for (auto& p : plans)
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

	for (size_t index = 0; index < nPlayers; ++index)
	{
		d.team = (byte)(index + 1);
		for (auto i = ents[index].begin(), end = ents[index].end(); i != end; ++i)
		{
			if (i->getHP() <= 0)
			{
				d.id = i.GetID();
				const float exprad = i->getExplosionRadius();
				if (exprad > 0.0f)
				{
					KillEnts(i->getPos(), exprad, i->getExplosionDamage(), i->getTeam());
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

		w.writeShort((short)PacketType::GameDeath);

		for (const auto& d : dEv)
		{
			w.writeByte(d.team);
			w.writeInt(d.id);
		}
		w.writeByte(0);

		serv.SendContainerAllReliable(std::move(c));
	}
}
void LocalMapServer::Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity* culprit)
{
	DataContainer c = serv.GetConRelSmall();
	ContainerWriter w(c);
	w.writeShort((short)PacketType::GamePlanetCapture);
	w.writeByte((char)pID);
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

	w.writeShort((short)PacketType::GameClock);
	w.writeFloat(curTime);

	serv.SendContainerAllUnreliable(std::move(con));
}

bool LocalMapServer::isAlive(byte team) const
{
	if (ents[team - 1].length() != 0)
		return true;

	//check planet
	for (const auto& p : plans)
	{
		if (p->getTeam() == team)
			return true;
	}
	return false;
}

void LocalMapServer::AddToGroup(byte team, int group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::GameAddToGroup);
	w.writeByte(team);
	w.writeInt(group);

	serv.SendContainerAllReliable(std::move(con));

	Map::AddToGroup(team, group);
}
void LocalMapServer::MakeGroup(byte team, int group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::GameMakeGroup);
	w.writeByte(team);
	w.writeInt(group);

	serv.SendContainerAllReliable(std::move(con));

	Map::MakeGroup(team, group);
}
void LocalMapServer::SelectGroup(byte team, int group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::GameSelectGroup);
	w.writeByte(team);
	w.writeInt(group);

	serv.SendContainerAllReliable(std::move(con));

	Map::SelectGroup(team, group);
}
void LocalMapServer::DeleteGroup(byte team, int group)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::GameDeleteGroup);
	w.writeByte(team);
	w.writeInt(group);

	serv.SendContainerAllReliable(std::move(con));

	Map::DeleteGroup(team, group);
}
void LocalMapServer::SelectAll(byte team)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::GameSelectAll);
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(con));

	Map::SelectAll(team);
}

bool LocalMapServer::HandleSurrender(byte team)
{
	bool bValid = false;
	if (ents[team - 1].length() != 0)
		bValid = true;

	if (!bValid)
	{
		// cheack plantes
		for (const auto& p : plans)
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

	w.writeShort((short)PacketType::GameTeamSurrender);
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(con));

	// kill entities
	while (ents[team - 1].length() != 0)
	{
		Event_EntityKilled(*ents[team - 1].begin());
		ents[team - 1].begin().remove();
	}

	// free planets
	for (auto& p : plans)
	{
		if (p->getTeam() == team)
		{
			p->takeOver(0);
			Event_PlanetCaptured(p->getID(), 0, team, nullptr);
		}
	}
	return true;
}

void LocalMapServer::HandleClientClanRequest(byte asking, byte team)
{
	if (gameType != GameType::UnholyAlliance)
		return;

	if (team == 0 || team > nPlayers)
		return;

	if (asking == team)
		return;

	if (CountAllies(asking) >= 2)
		return;

	const ClanInfo i = clanInfo[asking - 1][team - 1];
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
void LocalMapServer::HandleClientClanDestroy(byte asking, byte team)
{
	if (gameType != GameType::UnholyAlliance)
		return;

	if (team == 0 || team > nPlayers)
		return;

	if (asking == team)
		return;

	const ClanInfo i = clanInfo[asking - 1][team - 1];

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

void  LocalMapServer::SendAllyRequest(byte from, byte to)
{
	clanInfo[from - 1][to - 1] = ClanInfo::awaiting;
	clanInfo[to - 1][from - 1] = ClanInfo::sendRequest;

	SendClanUpdate(from, to);
}
void  LocalMapServer::SendAllyFormed(byte t1, byte t2)
{
	clanInfo[t1 - 1][t2 - 1] = ClanInfo::Ally;
	clanInfo[t2 - 1][t1 - 1] = ClanInfo::Ally;

	SendClanUpdate(t1, t2);
}
void  LocalMapServer::SendAllyResolve(byte t1, byte t2)
{
	clanInfo[t1 - 1][t2 - 1] = ClanInfo::noAlly;
	clanInfo[t2 - 1][t1 - 1] = ClanInfo::noAlly;

	SendClanUpdate(t1, t2);
}
void  LocalMapServer::SendAllyDeny(byte from, byte to)
{
	clanInfo[from - 1][to - 1] = ClanInfo::noAlly;
	clanInfo[to - 1][from - 1] = ClanInfo::noAlly;

	SendClanUpdate(from, to);
}

void LocalMapServer::SendClanUpdate(byte t1, byte t2)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::GameClanChange);
	w.writeByte(t1);
	w.writeByte((byte)(clanInfo[t1 - 1][t2 - 1]));
	w.writeByte(t2);
	w.writeByte((byte)(clanInfo[t2 - 1][t1 - 1]));

	serv.SendContainerAllReliable(std::move(con));
}

void LocalMapServer::HandleClientSpawntype(ContainerReader& r, byte team)
{
	MapObject::EntityType et = (MapObject::EntityType)r.readByte();
	if (et > MapObject::etSaboteur)
		throw std::domain_error("HandleClientSpawntype");

	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::GameSetPlanetSpawntpye);
	w.writeByte((char)et);
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(con));

	Map::SetPlanetSpawnType(team, et);
}

bool LocalMapServer::FilterEntityType(byte team, MapObject::EntityType et)
{
	if ((unsigned)et >= MapObject::etNone)
		return false;

	// check for any ent select first

	bool bEntSelect = false;

	for (const auto& e : ents[team - 1])
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
		for (const auto& p : plans)
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

	w.writeShort((short)PacketType::GameFilterEntityType);
	w.writeByte(team);
	w.writeByte(et);

	serv.SendContainerAllReliable(std::move(con));

	return LocalMap::FilterEntityType(team,et);
}
void LocalMapServer::SelectAllEntityType(byte team, MapObject::EntityType et)
{
	if ((unsigned)et >= MapObject::etNone)
		return;

	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::GameAllEntityType);
	w.writeByte(team);
	w.writeByte(et);

	serv.SendContainerAllReliable(std::move(con));

	LocalMap::SelectAllEntityType(team, et);
}
void LocalMapServer::HandleClientPlanDefense(ContainerReader& r, byte team)
{
	DataContainer con = serv.GetConRelSmall();
	ContainerWriter w(con);

	w.writeShort((short)PacketType::ServerPlanDefense);
	w.writeByte(team);

	serv.SendContainerAllReliable(std::move(con));

	Map::SetAllPlanetsOnDefense(team);
}