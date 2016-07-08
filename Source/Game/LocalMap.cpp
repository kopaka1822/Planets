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
	LoadMapComponents(planets, spawns);
}
LocalMap::LocalMap(int nPlayers, int nPlanets, float mWi, float mHei, Map::GameType ty, std::vector<byte> clns)
	:
	Map(nPlayers,nPlanets,mWi,mHei,ty,clns)
{}
void LocalMap::LoadMapComponents(const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns)
{
	srand(0); //same seed for spawn process
	RefreshGrid();
	for (unsigned int i = 0; i < planets.size(); i++)
	{
		plans.push_back(new LocalPlanet(planets[i], i));
	}

	for (const auto& s : spawns)
	{
		//Later
		for (int i = 0; i < s.nUnits; i++)
		{
			if (TryEntitySpawn(PointF{ s.x, s.y }, s.team, 0, MapObject::targetType::tgInvalid, PointF(), -1, false, 800.0f,(MapObject::entityType)s.type))
			{
				grid.AddEntity(&(*(ents[s.team - 1].back())), ents[s.team - 1].back()->GetPos());
			}
		}
	}

	srand((unsigned int)time(nullptr)); //different seed for ingame action
}
bool LocalMap::FilterEntityType(byte team, MapObject::entityType et)
{
	bool bAnySelect = false;
	bool bEntSelect = false;

	for (const auto& e : ents[team - 1])
	{
		if (e.selected())
		{
			bAnySelect = true;
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
				if (p->selected())
				{
					bAnySelect = true;
					if (p->GetEntityType() == et)
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

	return true;
}
bool LocalMap::Select(PointF center, float r2, byte team)
{
	if (Map::Select(center, r2, team))
	{
		//there is something to select
		for (auto& e : ents[team - 1])
		{
			float er2 = (e.GetPos() - center).lengthSq();

			if (er2 <= r2)
			{
				e.forceSelect();
			}
			else
			{
				e.deselect();
			}
		}
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
		return true;
	}
	else
	{
		return false;
	}
}
bool LocalMap::Click(PointF pt, byte team)
{
	//determine target type
	bool planetTargeted = false;

	MapEntity::targetType t = MapEntity::tgPoint;
	const MapPlanet* obj = GetColPlan(pt);
	if (obj != nullptr)
	{
		pt.x = obj->GetID();
		planetTargeted = true;
		if (obj->GetTeam() == team)
		{
			t = MapObject::tgPlanetDefend;
		}
		else
		{
			t = MapObject::tgPlanet;
		}
	}
	bool uselected = false; //units selected

	for (auto& e : ents[team - 1])
	{
		if (e.selected())
		{
			e.setTarget(pt, t);
			uselected = true;
		}

	}

	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
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
			plans[(unsigned int)pt.x]->forceSelect();
		}
		return false; // no planet target -> only selected
	}
	else
	{
		return planetTargeted;
	}
}

byte LocalMap::GameEnd() const
{
	// find first surviving team
	size_t surv = 0;
	for (; surv < nPlayers; surv++)
	{
		if (ents[surv].length() > 0)
			break;

		// search planet
		for (size_t i = 0; i < nPlans; i++)
			if (plans[i]->GetTeam() == surv + 1)
				goto end; // exit both loops
	}
end:

	if (surv >= nPlayers)
		return 0; // noone alive

	size_t other = 0;
	for (other = surv + 1; other < nPlayers; other++)
	{
		if (isAlly(surv + 1, other + 1))
			continue;

		if (ents[other].length() > 0)
			return 0;

		for (size_t i = 0; i < nPlans; i++)
			if (plans[i]->GetTeam() == other + 1)
				return 0;
	}
	return surv + 1; // only he and his allies are alive
}
LocalMap::~LocalMap()
{}
bool LocalMap::TryEntitySpawn(const PointF& c, const byte team, float r, MapObject::targetType ttype,
	const PointF& target, int group, bool isSelected, float maxR, MapObject::entityType entType)
{
	PointF d;
	const int randHalf = RAND_MAX / 2;
	std::function<bool(const PointF&)> isColliding = [this](const PointF& p)
	{
		//inside map?
		if (borderCol(p))
			return true;

		if (GetColPlan(p) != nullptr)
			return true;

		//entity collision
		return (GetColEnt(p,-1,0) != nullptr);
	};

	do
	{
		r += 1.0f;
		if (r > maxR)
			return false;

		d = PointF(float(rand() - randHalf), float(rand() - randHalf)).normalize() * r;
	} while (isColliding(d + c));

	unsigned int id = ents[team - 1].lastID() + 1;

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

	muEnts.Lock();
	ents[team - 1].add(newEnt);
	muEnts.Unlock();
	return true;
}
void LocalMap::UpdateEnts(const float dt)
{
	const float ds = dt * float(MapEntity::SPEED);

	std::function<void(EntIterator, EntIterator)> updFunc = [this, ds, dt](EntIterator start, EntIterator end)
	{
		if (start == end)
			return;

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
			SetEntPosition(*i,dt);
			AttackNearby(*i);
		}
	};

	UpdateEntsWithUpdater(updFunc);

	for (size_t index = 0; index < nPlayers; ++index)
	{
		for (auto i = ents[index].begin(), end = ents[index].end(); i != end; ++i)
		{
			if (i->GetHP() <= 0)
			{
				Map::Event_EntityKilled(*i);

				float exprad = i->GetExplosionRadius();
				if (exprad > 0.0f)
				{
					// nearby ents are going to die
					KillEnts(i->GetPos(), exprad,i->GetExplosionDamage(),i->GetTeam());
				}

				muEnts.Lock();
				i.remove();
				muEnts.Unlock();
			}
		}
	}
}
MapEntity* LocalMap::GetEnemyEnt(const PointF& pt, const byte team)
{
	for (auto& e : grid.GetEntities(pt))
	{
		if (!isAlly(team, e->GetTeam()))
		{
			if (e->isNearby(pt))
			{
				return e;
			}
		}
	}
	return nullptr;
}
void LocalMap::AttackNearby(MapEntity& curEnt)
{
	if (!curEnt.HasDamage())
		return;

	if (curEnt.AttacksEntities())
	{
		MapEntity* obj = GetEnemyEnt(curEnt.GetPos(), curEnt.GetTeam());
		if (obj != nullptr)
		{
			obj->TakeDamage(curEnt.GetDamage());
			return;
		}
	}

	if (curEnt.AttacksPlanets())
	{
		for (auto& p : plans)
		{
			if (p->isNearby(curEnt.GetPos()))
			{
				const byte pTeam = p->GetTeam();
				if (!isAlly(curEnt.GetTeam(),pTeam))
				{
					bool capture = false;
					if (curEnt.GetEntityType() == MapObject::etSaboteur)
					{
						p->Sabotage(curEnt.GetTeam());
						curEnt.SetHP(0);
						capture = true;
					}
					else
					{
						byte atkTeam = curEnt.GetTeam();
						if (isAlly(p->GetSubteam(), atkTeam))
						{
							/// push ally
							atkTeam = p->GetSubteam();
						}
						capture = p->TakeDamage(curEnt.GetDamage(), atkTeam);
					}

					if (capture)
					{
						Event_PlanetCaptured(p->GetID(), p->GetTeam(), pTeam, &curEnt);
						return;
					}
					else
					{
						Map::Event_PlanetAttacked(p->GetID(), curEnt);
						return;
					}
				}
			}
		}
	}
}

void LocalMap::KillEnts(const PointF& center, float radius, int damage, byte team)
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
	for (size_t i = 0; i < nPlayers; i++)
	{
		for (auto& e : ents[i])
		{
			if (r2 >= (e.GetPos() - center).lengthSq())
			{
				e.TakeDamage(damage);
			}
		}
	}

	// damage planets
	for (auto& p : plans)
	{
		if (p->isNearby(center))
		{
			byte pTeam = p->GetTeam();
			if (p->TakeDamage(damage, team))
			{
				Event_PlanetCaptured(p->GetID(), p->GetTeam(), pTeam, nullptr);
			}
		}
	}
}