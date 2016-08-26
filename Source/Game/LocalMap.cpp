#include "LocalMap.h"
#include <stdlib.h> //random
#include <time.h> //random
#include "../Utility/Line.h"
#include <functional> //lambda function
#include "LocalEntity.h"
#include "LocalPlanet.h"
#include <memory>
#include "LocalEntityBomber.h"
#include "LocalEntitySaboteur.h"
#include "LocalEntitySpeeder.h"
#include "../Utility/Timer.h"


LocalMap::LocalMap(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
	float width, float height, Map::GameType ty, std::vector<byte> clns)
	:
	Map(nPlayers,planets.size(),width,height,ty,clns)
{
	loadMapComponents(planets, spawns);
}
LocalMap::LocalMap(int nPlayers, int nPlanets, float mWi, float mHei, Map::GameType ty, std::vector<byte> clns)
	:
	Map(nPlayers,nPlanets,mWi,mHei,ty,clns)
{}
void LocalMap::loadMapComponents(const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns)
{
	srand(0); //same seed for spawn process
	refreshGrid();
	for (unsigned int i = 0; i < planets.size(); i++)
	{
		m_plans.push_back(new LocalPlanet(planets[i], i));
	}

	for (const auto& s : spawns)
	{
		//Later
		for (int i = 0; i < s.nUnits; i++)
		{
			if (tryEntitySpawn(PointF{ s.x, s.y }, s.team, 0, MapObject::TargetType::tgInvalid, PointF(), -1, false, 800.0f,(MapObject::EntityType)s.type))
			{
				m_grid.addEntity(&(*(m_ents[s.team - 1].back())), m_ents[s.team - 1].back()->getPos());
			}
		}
	}

	srand((unsigned int)time(nullptr)); //different seed for ingame action
}
bool LocalMap::filterEntityType(byte team, MapObject::EntityType et)
{
	bool bAnySelect = false;
	bool bEntSelect = false;

	for (const auto& e : m_ents[team - 1])
	{
		if (e.selected())
		{
			bAnySelect = true;
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
					bAnySelect = true;
					if (p->getEntityType() == et)
					{
						bEntSelect = true;
						break;
					}
				}
			}
		}
	}

	if (!bAnySelect)
		return false;

	if (!bEntSelect)
		return true;

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

	return true;
}
bool LocalMap::select(PointF center, float r2, byte team)
{
	if (Map::select(center, r2, team))
	{
		//there is something to select
		for (auto& e : m_ents[team - 1])
		{
			float er2 = (e.getPos() - center).lengthSq();

			if (er2 <= r2)
			{
				e.forceSelect();
			}
			else
			{
				e.deselect();
			}
		}
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
		return true;
	}
	else
	{
		return false;
	}
}
bool LocalMap::setTarget(PointF pt, byte team)
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
			t = MapObject::tgPlanetDefend;
		}
		else
		{
			t = MapObject::tgPlanet;
		}
	}
	bool uselected = false; //units selected

	for (auto& e : m_ents[team - 1])
	{
		if (e.selected())
		{
			e.setTarget(pt, t);
			uselected = true;
		}

	}

	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			if (p->selected())
			{
				p->setTarget(pt, t);
				uselected = true;
			}
		}
	}

	if (!uselected)
	{
		if (t == MapObject::tgPlanetDefend)
		{
			//select him
			m_plans[(unsigned int)pt.x]->forceSelect();
		}
		return false; // no planet target -> only selected
	}
	else
	{
		return planetTargeted;
	}
}

byte LocalMap::gameEnd() const
{
	// find first surviving m_team
	size_t surv = 0;
	for (; surv < m_nPlayers; surv++)
	{
		if (m_ents[surv].length() > 0)
			break;

		// search planet
		for (size_t i = 0; i < m_nPlans; i++)
			if (m_plans[i]->getTeam() == surv + 1)
				goto end; // exit both loops
	}
end:

	if (surv >= m_nPlayers)
		return 0; // noone alive

	size_t other = 0;
	for (other = surv + 1; other < m_nPlayers; other++)
	{
		if (isAlly(surv + 1, other + 1))
			continue;

		if (m_ents[other].length() > 0)
			return 0;

		for (size_t i = 0; i < m_nPlans; i++)
			if (m_plans[i]->getTeam() == other + 1)
				return 0;
	}
	return surv + 1; // only he and his allies are alive
}
LocalMap::~LocalMap()
{}
bool LocalMap::tryEntitySpawn(const PointF& c, const byte team, float r, MapObject::TargetType ttype,
	const PointF& target, int group, bool isSelected, float maxR, MapObject::EntityType entType)
{
	PointF d;
	const int randHalf = RAND_MAX / 2;
	std::function<bool(const PointF&)> isColliding = [this](const PointF& p)
	{
		//inside map?
		if (borderCol(p))
			return true;

		if (getColPlan(p) != nullptr)
			return true;

		//entity collision
		return (getColEnt(p,-1,0) != nullptr);
	};

	do
	{
		r += 1.0f;
		if (r > maxR)
			return false;

		d = PointF(float(rand() - randHalf), float(rand() - randHalf)).normalize() * r;
	} while (isColliding(d + c));

	unsigned int id = m_ents[team - 1].lastID() + 1;

	MapEntity* newEnt = nullptr;
	switch (entType)
	{
	case MapObject::etNormal:
		newEnt = new LocalEntity(d + c, team, ttype, target, group, isSelected, id, c);
		break;
	case MapObject::etBomber:
		newEnt = new LocalEntityBomber(d + c, team, ttype, target, group, isSelected, id, c);
		break;
	case MapObject::etSpeeder:
		newEnt = new LocalEntitySpeeder(d + c, team, ttype, target, group, isSelected, id, c);
		break;
	case MapObject::etSaboteur:
		newEnt = new LocalEntitySaboteur(d + c, team, ttype, target, group, isSelected, id, c);
		break;
	default:
		return false;
	}
	assert(newEnt != nullptr);

	m_muEnts.Lock();
	m_ents[team - 1].add(newEnt);
	m_muEnts.Unlock();
	return true;
}
void LocalMap::updateEnts(const float dt)
{
	const float ds = dt * float(MapEntity::SPEED);

	std::function<void(EntIterator, EntIterator)> updFunc = [this, ds, dt](EntIterator start, EntIterator end)
	{
		if (start == end)
			return;

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
			setEntPosition(*i,dt);
			attackNearby(*i);
		}
	};

	updateEntsWithUpdater(updFunc);

	for (size_t index = 0; index < m_nPlayers; ++index)
	{
		for (auto i = m_ents[index].begin(), end = m_ents[index].end(); i != end; ++i)
		{
			if (i->getHP() <= 0)
			{
				Map::Event_EntityKilled(*i);

				float exprad = i->getExplosionRadius();
				if (exprad > 0.0f)
				{
					// nearby ents are going to die
					killEnts(i->getPos(), exprad,i->getExplosionDamage(),i->getTeam());
				}

				m_muEnts.Lock();
				i.remove();
				m_muEnts.Unlock();
			}
		}
	}
}
MapEntity* LocalMap::getEnemyEnt(const PointF& pt, const byte team)
{
	for (auto& e : m_grid.getEntities(pt))
	{
		if (!isAlly(team, e->getTeam()))
		{
			if (e->isNearby(pt))
			{
				return e;
			}
		}
	}
	return nullptr;
}
void LocalMap::attackNearby(MapEntity& curEnt)
{
	if (!curEnt.hasDamage())
		return;

	if (curEnt.attacksEntities())
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
				if (!isAlly(curEnt.getTeam(),pTeam))
				{
					bool capture = false;
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
							/// push ally
							atkTeam = p->getSubteam();
						}
						capture = p->takeDamage(curEnt.getDamage(), atkTeam);
					}

					if (capture)
					{
						Event_PlanetCaptured(p->getID(), p->getTeam(), pTeam, &curEnt);
						return;
					}
					else
					{
						Map::Event_PlanetAttacked(p->getID(), curEnt);
						return;
					}
				}
			}
		}
	}
}

void LocalMap::killEnts(const PointF& center, float radius, int damage, byte team)
{
	const float r2 = radius * radius;
	/*
	const float off = grid.GetBoxSize();
	const float offhalf = off / 2.0f;


	float xStart = center.x - radius;
	float yStart = center.y - radius;
	float xEnd = center.x + radius;
	float yEnd = center.y + radius;

	for (float x = xStart; x < xEnd; x += off)
	{
		for (float y = yStart; y < yEnd; y += off)
		{
			for (auto& e : grid.GetEntities(PointF(x, y)))
			{
				if (r2 >= (e->GetPos() - center).lengthSq())
				{
					e->TakeDamage(damage);
				}
			}
		}
	}*/

	// we cant use the grid because there are some deleted entities inside..
	for (size_t i = 0; i < m_nPlayers; i++)
	{
		for (auto& e : m_ents[i])
		{
			if (r2 >= (e.getPos() - center).lengthSq())
			{
				e.takeDamage(damage);
			}
		}
	}

	// damage planets
	for (auto& p : m_plans)
	{
		if (p->isNearby(center))
		{
			byte pTeam = p->getTeam();
			if (p->takeDamage(damage, team))
			{
				Event_PlanetCaptured(p->getID(), p->getTeam(), pTeam, nullptr);
			}
		}
	}
}
