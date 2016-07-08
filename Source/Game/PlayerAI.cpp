#include "PlayerAI.h"
#include <cfloat>
#include "../Utility/LockGuard.h"

PlayerAI::PlayerAI(byte team, Map& map)
	:
	team(team),
	map(map),
	nPlanets(0)
{
	planTask.assign(map.nPlans, nullptr);
	planAtkTask.assign(map.nPlans, nullptr);

	for (size_t i = 0; i < map.nPlans; i++)
	{
		if (map.plans[i]->GetTeam() == team)
		{
			planTask[i] = new PlanetTask(*this, i);
			nPlanets++;
		}
	}

	// init nearby planets ??
	const float nearr = CalcNearByDistance(4); // in average 4 planets nearby
	const float near2 = nearr * nearr;
	nearbyPlanets.assign(map.nPlans, std::vector<PlanetID>());

	for (size_t i = 0; i < map.nPlans; i++)
	{
		const PointF& cen = map.getPlan(i).GetPos();
		for (size_t j = 0; j < map.nPlans; j++)
		{
			if (i != j)
			{
				if ((map.getPlan(j).GetPos() - cen).lengthSq() < near2)
				{
					nearbyPlanets[i].push_back(j);
				}
			}
		}
	}
	if (map.nPlans > 0)
	{
		for (auto& ent : map.ents[team - 1])
		{
			PlanetID pid = GetAttackPlan(ent.GetPos());
			ent.groupAssign(-2);
			AttackPlanet(-2, 1, pid);

		}
	}
}
PlayerAI::~PlayerAI()
{
	for (auto& t : planTask)
	{
		tool::safeDelete(t);
	}
	for (auto& t : planAtkTask)
	{
		tool::safeDelete(t);
	}
}
void PlayerAI::DoTurn(float dt)
{
	for (auto& t : planTask)
	{
		if (t)
			t->DoTask(dt);
	}
	for (auto& t : bombHunters)
	{
		if (t)
			t->DoTask(dt);
	}
	for (auto& t : huntTask)
	{
		if (t)
			t->DoTask(dt);
	}
}
PlanetID PlayerAI::GetPlanetInTrouble(PlanetID own)
{
	if (unsigned(own) >= planTask.size())
		return own;

	for (const auto& pid : nearbyPlanets[own])
	{
		assert(unsigned(pid) < planTask.size());
		if (planTask[pid] != nullptr)
		{
			if (planTask[pid]->isInTrouble())
				return pid;
		}
	}
	return own; // no planet in trouble
}
size_t PlayerAI::AttackPlanet(int group, size_t num, PlanetID target)
{
	assert(unsigned(target) < planAtkTask.size());
	if (planAtkTask[target] == nullptr)
	{
		planAtkTask[target] = new PlanetAttackTask(*this, target);
	}

	auto is = GroupTransfer(group, planAtkTask[target]->GetGroupID(), num);
	planAtkTask[target]->AddToGroup(is);
	planAtkTask[target]->Update();
	return is;
}
size_t PlayerAI::HelpPlanet(int group, size_t num, PlanetID target)
{
	assert(target < signed(planTask.size()));
	assert(planTask[target] != nullptr);
	if (planTask[target] == nullptr)
		return 0;

	auto is = GroupTransfer(group, target, num);
	planTask[target]->AddToGroup(is);
	planTask[target]->Update();
	return is;
}
size_t PlayerAI::GroupTransfer(int from, int to, size_t num)
{
	size_t is = 0;
	for (auto& e : map.ents[team - 1])
	{
		if (e.GetGroup() == from)
		{
			is++;
			e.groupAssign(to);
			if (is >= num)
				break;
		}
	}
	return is;
}
MapEntity* PlayerAI::GetBomberTarget(const MapEntity& bomber)
{
	assert(bomber.GetEntityType() == MapObject::etBomber);

	PointI huntPos = GetEnemyAccumulation(bomber.GetPos(), 250.0f, 10);

	if (huntPos != PointI(-1, -1))
	{
		// hunt an enemy in this box
		for (const auto& e : map.grid.GetEntitiesRaw(huntPos))
		{
			if (!map.isAlly(e->GetTeam(), team))
				return e;
		}
	}

	return nullptr;
}
MapEntity* PlayerAI::GetHuntTarget(const PointF& center, float radius)
{
	PointI enemyPos = GetEnemyAccumulation(center, radius, 1);

	if (enemyPos != PointI(-1, -1))
	{
		for (const auto& e : map.grid.GetEntitiesRaw(enemyPos))
		{
			if (!map.isAlly(e->GetTeam(), team))
				return e;
		}
	}

	return nullptr;
}
void PlayerAI::AddHunter(MapEntity& e)
{
	static const int MAX_HUNTERS = 10;

	if (huntTask.size() > MAX_HUNTERS)
	{
		// search nearest target
		HuntTask* best = huntTask.front().get();
		float minDist = FLT_MAX;
		for (const auto& t : huntTask)
		{
			//  only if group has a target
			if (t->GetTarget() != nullptr)
			{
				float dist = (e.GetPos() - t->GetPos()).lengthSq();
				if (minDist > dist)
				{
					minDist = dist;
					best = t.get();
				}
			}
		}

		best->AddToGroup(1);
		e.groupAssign(best->GetID());
	}
	else
	{
		huntTask.push_back(std::unique_ptr< HuntTask >(new HuntTask(*this, e)));
	}
}
void PlayerAI::Event_EntitySpawn(PlanetID pID, MapEntity& e) // thread safe
{
	if (e.GetTeam() == team && e.hasGroup())
	{
		if (e.GetEntityType() == MapObject::etBomber)
		{
			bombHunters.push_back(std::unique_ptr< BomberHuntTask >(new BomberHuntTask(*this, e)));
		}
		else if (e.GetEntityType() == MapObject::etSpeeder)
		{
			AddHunter(e);
		}
		else if (e.GetEntityType() == MapObject::etSaboteur)
		{
			// enity should be from any planet...
			assert(e.GetGroup() >= PlanetTask::ID_START && e.GetGroup() <= PlanetTask::ID_END);
			assert(unsigned(e.GetGroup()) < map.nPlans);
			assert(planTask[e.GetGroup()]);
			// attack with whole planet

			const auto group = e.GetGroup();
			planTask[group]->IncGroupSize(e.GetEntityType());
			int num = AttackPlanet(e.GetGroup(), planTask[group]->GetGroupSize(), GetWhitePlan(pID));
			planTask[group]->AddToGroup(-num);
		}
		else // no bomber and no saboteur
		{
			// enity should be from any planet...
			assert(e.GetGroup() >= PlanetTask::ID_START && e.GetGroup() <= PlanetTask::ID_END);
			if (e.GetGroup() >= PlanetTask::ID_START && e.GetGroup() <= PlanetTask::ID_END)
			{
				if (unsigned(e.GetGroup()) < map.nPlans)
				{
					// planet group
					assert(unsigned(e.GetGroup()) < planTask.size());
					if (planTask[e.GetGroup()] != nullptr)
					{
						planTask[e.GetGroup()]->IncGroupSize(e.GetEntityType());
					}
				}
			}
			// is that even possible?
			else if (e.GetGroup() >= PlanetAttackTask::ID_START && e.GetGroup() <= PlanetAttackTask::ID_END)
			{
				size_t tid = unsigned(e.GetGroup() - PlanetAttackTask::ID_START);
				if (tid < map.nPlans)
				{
					if (planAtkTask[tid] != nullptr)
					{
						planAtkTask[tid]->AddToGroup(1);
					}
				}
			}
		}
	}
}
void PlayerAI::Event_EntityKilled(MapEntity& e)  // thread safe
{
	if (e.GetTeam() == team)
	{
		if (e.GetGroup() >= PlanetTask::ID_START && e.GetGroup() <= PlanetTask::ID_END)
		{
			if ((unsigned)e.GetGroup() < map.nPlans)
			{
				// planet group
				assert(unsigned(e.GetGroup()) < planTask.size());
				if (planTask[e.GetGroup()] != nullptr)
				{
					planTask[e.GetGroup()]->DecGroupSize();
				}
			}
		}
		else if (e.GetGroup() >= PlanetAttackTask::ID_START && e.GetGroup() <= PlanetAttackTask::ID_END)
		{
			// attack task
			size_t tid = unsigned(e.GetGroup() - PlanetAttackTask::ID_START);
			if (tid < map.nPlans)
			{
				if (planAtkTask[tid] != nullptr)
				{
					planAtkTask[tid]->AddToGroup(-1);
				}
			}
		}
		else
		{
			// probably hunt task
			bombHunters.remove_if([&e](const std::unique_ptr<BomberHuntTask>& task){
				if (task->GetBomberID() == e.GetID())
					return true;
				else return false;
			});

			huntTask.remove_if([&e](std::unique_ptr<HuntTask>& task){
				if (task->GetID() == e.GetID())
				{
					task->DecGroupSize();
					if (task->GetGroupSize() == 0)
						return true;
				}
				return false;
			});
		}
	}
	else
	{
		// a target from Hunt task was killed?

		for (auto& t : bombHunters)
		{
			if (t->GetTarget())
			{
				if (t->GetTarget()->GetTeam() == e.GetTeam() && t->GetTarget()->GetID() == e.GetID())
				{
					t->SetTarget(nullptr);
				}
			}
		}

		for (auto& t : huntTask)
		{
			if (t->GetTarget())
			{
				if (t->GetTarget()->GetTeam() == e.GetTeam() && t->GetTarget()->GetID() == e.GetID())
				{
					t->SetTarget(nullptr);
				}
			}
		}
	}
}
void PlayerAI::Event_PlanetAttacked(PlanetID pID, const MapEntity& e) // not thread safe
{

}
void PlayerAI::Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity*) // not thread safe
{
	if (map.isAlly(newTeam, team))
	{
		LockGuard g(muEvent);

		if (newTeam == team)
		{
			if (planTask[pID] == nullptr)
			{
				nPlanets++;
				planTask[pID] = new PlanetTask(*this, pID);
			}
		}

		// resolve attack units
		if (planAtkTask[pID] != nullptr)
		{
			if (planTask[pID] != nullptr)
			{
				// transfer ALL units to new planet
				auto num = GroupTransfer(planAtkTask[pID]->GetGroupID(), pID, -1);
				planTask[pID]->AddToGroup(num);
			}
			else if (planAtkTask[pID]->GetGroupSize() > 0)
			{
				//  its you allies planet -> attack another one
				PlanetID attack = GetAttackPlan(map.getPlan(pID).GetPos());
				if (attack != pID)
				{
					AttackPlanet(planAtkTask[pID]->GetGroupID(), planAtkTask[pID]->GetGroupSize(), attack);
				}
				else
				{
					// just hunt down random entities
					int gid = planAtkTask[pID]->GetGroupID();

					for (auto& e : map.ents[team - 1])
						if (e.GetGroup() == gid)
							AddHunter(e);
				}
			}

			planAtkTask[pID]->Destroy();
			delete planAtkTask[pID];
			planAtkTask[pID] = nullptr;
		}
		g.Unlock();
	}
	else if (oldTeam == team)
	{
		// Lost planet :/
		LockGuard g(muEvent);
		// delete task
		if (planTask[pID] != nullptr)
		{
			// destroy -> attack planet to get it back!
			planTask[pID]->Destroy();
			delete planTask[pID];
			planTask[pID] = nullptr;

		}
		g.Unlock();
		nPlanets--;
	}
}
PointF PlayerAI::GetMidPlanetPos(byte team)
{
	PointF midPos;
	byte i = 0;
	for (const auto plan : map.plans)
	{
		if (plan->GetTeam() == team)
		{
			midPos += plan->GetPos();
			i++;
		}
	}
	if (i == 0)
		i = 1;
	return midPos / i;
}
PlanetID PlayerAI::GetWhitePlan(PlanetID own)
{
	if (unsigned(own) >= map.nPlans)
		return own;

	PlanetID best = own;
	float bestValue = FLT_MAX;

	for (const auto& plan : nearbyPlanets[own])
	{
		if (map.getPlan(plan).GetTeam() == 0 && map.getPlan(plan).GetSubteam() == 0)
		{
			float value = (map.getPlan(own).GetPos() - map.getPlan(plan).GetPos()).lengthSq() * (rand() % 2 + 1);
			if (value < bestValue)
			{
				bestValue = value;
				best = plan;
			}
		}
	}
	return best;
}
PointI PlayerAI::GetEnemyAccumulation(const PointF& pos, float r, size_t lowerLimit)
{
	return map.grid.GetEnemyAccumulationPoint(team, pos, r, lowerLimit);
}
const FastVector<MapEntity*>& PlayerAI::GetEntitiesGrid(PointF pos) const
{
	return map.grid.GetEntities(pos);
}
PlanetID PlayerAI::GetWellDefendedNear(PlanetID own)
{
	if (unsigned(own) >= map.nPlans)
		return own;

	for (const auto& plan : nearbyPlanets[own])
	{
		if (GetEnemyEntsInDefR(map.getPlan(plan).GetID(), map.getPlan(own).GetTeam()) >= BOMBER_SEND_SIZE)
			return plan;
	}
	return own;
}
size_t PlayerAI::GetOwnEntsInDefR(PlanetID pID, byte team) const
{
	if (team == 0 || team > map.nPlayers)
		return 1;

	if (unsigned(pID) >= map.nPlans)
		return 1;
	return std::max(size_t(1), map.grid.CountUnits(size_t(team), map.getPlan(pID).GetPos(), map.getPlan(pID).GetDefenseRadius()));
}
size_t PlayerAI::GetAlliEntsInDefR(PlanetID pID, byte team) const
{
	if (team == 0 || team > map.nPlayers)
		return 1;

	if (unsigned(pID) >= map.nPlans)
		return 1;
	return std::max(size_t(1), map.grid.CountAllyUnits(team, map.getPlan(pID).GetPos(), map.getPlan(pID).GetDefenseRadius()));
}
size_t PlayerAI::GetEnemyEntsInDefR(PlanetID pID, byte team) const
{
	if (team == 0 || team > map.nPlayers)
		return 1;

	if (unsigned(pID) >= map.nPlans)
		return 1;
	return std::max(size_t(1), map.grid.CountEnemyUnits(team, map.getPlan(pID).GetPos(), map.getPlan(pID).GetDefenseRadius()));
}
size_t PlayerAI::GetOwnEntsInR(PointF pos, byte team, float r) const
{
	return map.grid.CountUnits(team, pos, r);
}
size_t PlayerAI::GetEnemyEntsInR(PointF pos, byte team, float r) const
{
	return map.grid.CountEnemyUnits(team, pos, r);
}
size_t PlayerAI::GetAlliEntsInR(PointF pos, byte team, float r) const
{
	return map.grid.CountEnemyUnits(team, pos, r);
}
PlanetID PlayerAI::GetAttackPlan(PointF entPos) const
{
	PlanetID fav = map.plans[0]->GetID();

	float lastDist = FLT_MAX;
	for (const auto& p : map.plans)
	{
		if (!map.isAlly(p->GetTeam(), team))
		{
			float d =
				(entPos - p->GetPos()).lengthSq()
				* GetEnemyEntsInDefR(p->GetID(), team);


			if (d < lastDist)
			{
				lastDist = d;
				fav = p->GetID();
			}
		}
	}

	return fav;
}
float PlayerAI::CalcNearByDistance(int average)
{
	float* nearest = (float*)malloc((average + 1)*sizeof(float));

	float avg = 0;
	for (auto& plan : map.plans)
	{
		for (int i = average; i >= 0; i--)
			nearest[i] = 99999999999.0f;

		for (auto& plan2 : map.plans)
		{
			if (plan->GetID() == plan2->GetID())
				continue;
			for (int i = average - 1; i >= 0; i--)
			{
				float dist = (plan->GetPos() - plan2->GetPos()).length();
				if (dist < nearest[i] && i > 0)
				{
					nearest[i + 1] = nearest[i];
				}
				else
				{
					nearest[i + 1] = dist;
					break;
				}
			}
		}
		avg += nearest[average - 1];
	}
	free(nearest);
	return avg / map.nPlans;
}
