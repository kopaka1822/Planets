#include "PlayerAI.h"
#include <cfloat>
#include "../Utility/LockGuard.h"

PlayerAI::PlayerAI(byte team, Map& map)
	:
	m_team(team),
	m_map(map),
	m_nPlanets(0)
{
	m_planTask.assign(map.m_nPlans, nullptr);
	m_planAtkTask.assign(map.m_nPlans, nullptr);

	for (size_t i = 0; i < map.m_nPlans; i++)
	{
		if (map.m_plans[i]->getTeam() == team)
		{
			m_planTask[i] = new PlanetTask(*this, i);
			m_nPlanets++;
		}
	}

	// init nearby planets ??
	const float nearr = calcNearByDistance(4); // in average 4 planets nearby
	const float near2 = nearr * nearr;
	m_nearbyPlanets.assign(map.m_nPlans, std::vector<PlanetID>());

	for (size_t i = 0; i < map.m_nPlans; i++)
	{
		const PointF& cen = map.getPlan(i).getPos();
		for (size_t j = 0; j < map.m_nPlans; j++)
		{
			if (i != j)
			{
				if ((map.getPlan(j).getPos() - cen).lengthSq() < near2)
				{
					m_nearbyPlanets[i].push_back(j);
				}
			}
		}
	}
	if (map.m_nPlans > 0)
	{
		for (auto& ent : map.m_ents[team - 1])
		{
			PlanetID pid = getAttackPlan(ent.getPos());
			ent.groupAssign(-2);
			attackPlanet(-2, 1, pid);

		}
	}
}
PlayerAI::~PlayerAI()
{
	for (auto& t : m_planTask)
	{
		tool::safeDelete(t);
	}
	for (auto& t : m_planAtkTask)
	{
		tool::safeDelete(t);
	}
}
void PlayerAI::doTurn(float dt)
{
	for (auto& t : m_planTask)
	{
		if (t)
			t->doTask(dt);
	}
	for (auto& t : m_bombHunters)
	{
		if (t)
			t->doTask(dt);
	}
	for (auto& t : m_huntTask)
	{
		if (t)
			t->doTask(dt);
	}
}
PlanetID PlayerAI::getPlanetInTrouble(PlanetID own)
{
	if (unsigned(own) >= m_planTask.size())
		return own;

	for (const auto& pid : m_nearbyPlanets[own])
	{
		assert(unsigned(pid) < m_planTask.size());
		if (m_planTask[pid] != nullptr)
		{
			if (m_planTask[pid]->isInTrouble())
				return pid;
		}
	}
	return own; // no planet in trouble
}
size_t PlayerAI::attackPlanet(int group, size_t num, PlanetID target)
{
	assert(unsigned(target) < m_planAtkTask.size());
	if (m_planAtkTask[target] == nullptr)
	{
		m_planAtkTask[target] = new PlanetAttackTask(*this, target);
	}

	auto is = groupTransfer(group, m_planAtkTask[target]->getGroupID(), num);
	m_planAtkTask[target]->addToGroup(is);
	m_planAtkTask[target]->update();
	return is;
}
size_t PlayerAI::helpPlanet(int group, size_t num, PlanetID target)
{
	assert(target < signed(m_planTask.size()));
	assert(m_planTask[target] != nullptr);
	if (m_planTask[target] == nullptr)
		return 0;

	auto is = groupTransfer(group, target, num);
	m_planTask[target]->addToGroup(is);
	m_planTask[target]->update();
	return is;
}
size_t PlayerAI::groupTransfer(int from, int to, size_t num)
{
	size_t is = 0;
	for (auto& e : m_map.m_ents[m_team - 1])
	{
		if (e.getGroup() == from)
		{
			is++;
			e.groupAssign(to);
			if (is >= num)
				break;
		}
	}
	return is;
}
MapEntity* PlayerAI::getBomberTarget(const MapEntity& bomber)
{
	assert(bomber.getEntityType() == MapObject::etBomber);

	PointI huntPos = getEnemyAccumulation(bomber.getPos(), 250.0f, 10);

	if (huntPos != PointI(-1, -1))
	{
		// hunt an enemy in this box
		for (const auto& e : m_map.m_grid.getEntitiesRaw(huntPos))
		{
			if (!m_map.isAlly(e->getTeam(), m_team))
				return e;
		}
	}

	return nullptr;
}
MapEntity* PlayerAI::getHuntTarget(const PointF& center, float radius)
{
	PointI enemyPos = getEnemyAccumulation(center, radius, 1);

	if (enemyPos != PointI(-1, -1))
	{
		for (const auto& e : m_map.m_grid.getEntitiesRaw(enemyPos))
		{
			if (!m_map.isAlly(e->getTeam(), m_team))
				return e;
		}
	}

	return nullptr;
}
void PlayerAI::addHunter(MapEntity& e)
{
	static const int MAX_HUNTERS = 10;

	if (m_huntTask.size() > MAX_HUNTERS)
	{
		// search nearest target
		HuntTask* best = m_huntTask.front().get();
		float minDist = FLT_MAX;
		for (const auto& t : m_huntTask)
		{
			//  only if m_group has a target
			if (t->getTarget() != nullptr)
			{
				float dist = (e.getPos() - t->getPos()).lengthSq();
				if (minDist > dist)
				{
					minDist = dist;
					best = t.get();
				}
			}
		}

		best->addToGroup(1);
		e.groupAssign(best->getID());
	}
	else
	{
		m_huntTask.push_back(std::unique_ptr< HuntTask >(new HuntTask(*this, e)));
	}
}
void PlayerAI::Event_EntitySpawn(PlanetID pID, MapEntity& e) // thread safe
{
	if (e.getTeam() == m_team && e.hasGroup())
	{
		if (e.getEntityType() == MapObject::etBomber)
		{
			m_bombHunters.push_back(std::unique_ptr< BomberHuntTask >(new BomberHuntTask(*this, e)));
		}
		else if (e.getEntityType() == MapObject::etSpeeder)
		{
			addHunter(e);
		}
		else if (e.getEntityType() == MapObject::etSaboteur)
		{
			// enity should be from any planet...
			assert(e.getGroup() >= PlanetTask::ID_START && e.getGroup() <= PlanetTask::ID_END);
			assert(unsigned(e.getGroup()) < m_map.m_nPlans);
			assert(m_planTask[e.getGroup()]);
			// attack with whole planet

			const auto group = e.getGroup();
			m_planTask[group]->incGroupSize(e.getEntityType());
			int num = attackPlanet(e.getGroup(), m_planTask[group]->getGroupSize(), getWhitePlan(pID));
			m_planTask[group]->addToGroup(-num);
		}
		else // no bomber and no saboteur
		{
			// enity should be from any planet...
			assert(e.getGroup() >= PlanetTask::ID_START && e.getGroup() <= PlanetTask::ID_END);
			if (e.getGroup() >= PlanetTask::ID_START && e.getGroup() <= PlanetTask::ID_END)
			{
				if (unsigned(e.getGroup()) < m_map.m_nPlans)
				{
					// planet m_group
					assert(unsigned(e.getGroup()) < m_planTask.size());
					if (m_planTask[e.getGroup()] != nullptr)
					{
						m_planTask[e.getGroup()]->incGroupSize(e.getEntityType());
					}
				}
			}
			// is that even possible?
			else if (e.getGroup() >= PlanetAttackTask::ID_START && e.getGroup() <= PlanetAttackTask::ID_END)
			{
				size_t tid = unsigned(e.getGroup() - PlanetAttackTask::ID_START);
				if (tid < m_map.m_nPlans)
				{
					if (m_planAtkTask[tid] != nullptr)
					{
						m_planAtkTask[tid]->addToGroup(1);
					}
				}
			}
		}
	}
}
void PlayerAI::Event_EntityKilled(MapEntity& e)  // thread safe
{
	if (e.getTeam() == m_team)
	{
		if (e.getGroup() >= PlanetTask::ID_START && e.getGroup() <= PlanetTask::ID_END)
		{
			if ((unsigned)e.getGroup() < m_map.m_nPlans)
			{
				// planet m_group
				assert(unsigned(e.getGroup()) < m_planTask.size());
				if (m_planTask[e.getGroup()] != nullptr)
				{
					m_planTask[e.getGroup()]->decGroupSize();
				}
			}
		}
		else if (e.getGroup() >= PlanetAttackTask::ID_START && e.getGroup() <= PlanetAttackTask::ID_END)
		{
			// attack task
			size_t tid = unsigned(e.getGroup() - PlanetAttackTask::ID_START);
			if (tid < m_map.m_nPlans)
			{
				if (m_planAtkTask[tid] != nullptr)
				{
					m_planAtkTask[tid]->addToGroup(-1);
				}
			}
		}
		else
		{
			// probably hunt task
			m_bombHunters.remove_if([&e](const std::unique_ptr<BomberHuntTask>& task){
				if (task->getBomberID() == e.getID())
					return true;
				else return false;
			});

			m_huntTask.remove_if([&e](std::unique_ptr<HuntTask>& task){
				if (task->getID() == e.getID())
				{
					task->decGroupSize();
					if (task->getGroupSize() == 0)
						return true;
				}
				return false;
			});
		}
	}
	else
	{
		// a target from Hunt task was killed?

		for (auto& t : m_bombHunters)
		{
			if (t->getTarget())
			{
				if (t->getTarget()->getTeam() == e.getTeam() && t->getTarget()->getID() == e.getID())
				{
					t->setTarget(nullptr);
				}
			}
		}

		for (auto& t : m_huntTask)
		{
			if (t->getTarget())
			{
				if (t->getTarget()->getTeam() == e.getTeam() && t->getTarget()->getID() == e.getID())
				{
					t->setTarget(nullptr);
				}
			}
		}
	}
}
void PlayerAI::Event_PlanetAttacked(PlanetID pID, const MapEntity& e) // not thread safe
{

}
void PlayerAI::Event_PlanetCaptured(PlanetID pID, TeamID newTeam, TeamID oldTeam, const MapEntity*) // not thread safe
{
	if (m_map.isAlly(newTeam, m_team))
	{
		LockGuard g(m_muEvent);

		if (newTeam == m_team)
		{
			if (m_planTask[pID] == nullptr)
			{
				m_nPlanets++;
				m_planTask[pID] = new PlanetTask(*this, pID);
			}
		}

		// resolve attack units
		if (m_planAtkTask[pID] != nullptr)
		{
			if (m_planTask[pID] != nullptr)
			{
				// transfer ALL units to new planet
				auto num = groupTransfer(m_planAtkTask[pID]->getGroupID(), pID, -1);
				m_planTask[pID]->addToGroup(num);
			}
			else if (m_planAtkTask[pID]->getGroupSize() > 0)
			{
				//  its you allies planet -> attack another one
				PlanetID attack = getAttackPlan(m_map.getPlan(pID).getPos());
				if (attack != pID)
				{
					attackPlanet(m_planAtkTask[pID]->getGroupID(), m_planAtkTask[pID]->getGroupSize(), attack);
				}
				else
				{
					// just hunt down random entities
					int gid = m_planAtkTask[pID]->getGroupID();

					for (auto& e : m_map.m_ents[m_team - 1])
						if (e.getGroup() == gid)
							addHunter(e);
				}
			}

			m_planAtkTask[pID]->destroy();
			delete m_planAtkTask[pID];
			m_planAtkTask[pID] = nullptr;
		}
		g.Unlock();
	}
	else if (oldTeam == m_team)
	{
		// Lost planet :/
		LockGuard g(m_muEvent);
		// delete task
		if (m_planTask[pID] != nullptr)
		{
			// destroy -> attack planet to get it back!
			m_planTask[pID]->destroy();
			delete m_planTask[pID];
			m_planTask[pID] = nullptr;

		}
		g.Unlock();
		m_nPlanets--;
	}
}
PointF PlayerAI::getMidPlanetPos(byte team)
{
	PointF midPos;
	byte i = 0;
	for (const auto plan : m_map.m_plans)
	{
		if (plan->getTeam() == team)
		{
			midPos += plan->getPos();
			i++;
		}
	}
	if (i == 0)
		i = 1;
	return midPos / i;
}
PlanetID PlayerAI::getWhitePlan(PlanetID own)
{
	if (unsigned(own) >= m_map.m_nPlans)
		return own;

	PlanetID best = own;
	float bestValue = FLT_MAX;

	for (const auto& plan : m_nearbyPlanets[own])
	{
		if (m_map.getPlan(plan).getTeam() == 0 && m_map.getPlan(plan).getSubteam() == 0)
		{
			float value = (m_map.getPlan(own).getPos() - m_map.getPlan(plan).getPos()).lengthSq() * (rand() % 2 + 1);
			if (value < bestValue)
			{
				bestValue = value;
				best = plan;
			}
		}
	}
	return best;
}
PointI PlayerAI::getEnemyAccumulation(const PointF& pos, float r, size_t lowerLimit)
{
	return m_map.m_grid.getEnemyAccumulationPoint(m_team, pos, r, lowerLimit);
}
const FastVector<MapEntity*>& PlayerAI::getEntitiesGrid(PointF pos) const
{
	return m_map.m_grid.getEntities(pos);
}
PlanetID PlayerAI::getWellDefendedNear(PlanetID own)
{
	if (unsigned(own) >= m_map.m_nPlans)
		return own;

	for (const auto& plan : m_nearbyPlanets[own])
	{
		if (getEnemyEntsInDefR(m_map.getPlan(plan).getID(), m_map.getPlan(own).getTeam()) >= BOMBER_SEND_SIZE)
			return plan;
	}
	return own;
}
size_t PlayerAI::getOwnEntsInDefR(PlanetID pID, byte team) const
{
	if (team == 0 || team > m_map.m_nPlayers)
		return 1;

	if (unsigned(pID) >= m_map.m_nPlans)
		return 1;
	return std::max(size_t(1), m_map.m_grid.countUnits(size_t(team), m_map.getPlan(pID).getPos(), m_map.getPlan(pID).getDefenseRadius()));
}
size_t PlayerAI::getAlliEntsInDefR(PlanetID pID, byte team) const
{
	if (team == 0 || team > m_map.m_nPlayers)
		return 1;

	if (unsigned(pID) >= m_map.m_nPlans)
		return 1;
	return std::max(size_t(1), m_map.m_grid.countAllyUnits(team, m_map.getPlan(pID).getPos(), m_map.getPlan(pID).getDefenseRadius()));
}
size_t PlayerAI::getEnemyEntsInDefR(PlanetID pID, byte team) const
{
	if (team == 0 || team > m_map.m_nPlayers)
		return 1;

	if (unsigned(pID) >= m_map.m_nPlans)
		return 1;
	return std::max(size_t(1), m_map.m_grid.countEnemyUnits(team, m_map.getPlan(pID).getPos(), m_map.getPlan(pID).getDefenseRadius()));
}
size_t PlayerAI::getOwnEntsInR(PointF pos, byte team, float r) const
{
	return m_map.m_grid.countUnits(team, pos, r);
}
size_t PlayerAI::getEnemyEntsInR(PointF pos, byte team, float r) const
{
	return m_map.m_grid.countEnemyUnits(team, pos, r);
}
size_t PlayerAI::getAlliEntsInR(PointF pos, byte team, float r) const
{
	return m_map.m_grid.countEnemyUnits(team, pos, r);
}
PlanetID PlayerAI::getAttackPlan(PointF entPos) const
{
	PlanetID fav = m_map.m_plans[0]->getID();

	float lastDist = FLT_MAX;
	for (const auto& p : m_map.m_plans)
	{
		if (!m_map.isAlly(p->getTeam(), m_team))
		{
			float d =
				(entPos - p->getPos()).lengthSq()
				* getEnemyEntsInDefR(p->getID(), m_team);


			if (d < lastDist)
			{
				lastDist = d;
				fav = p->getID();
			}
		}
	}

	return fav;
}
float PlayerAI::calcNearByDistance(int average)
{
	float* nearest = (float*)malloc((average + 1)*sizeof(float));

	float avg = 0;
	for (auto& plan : m_map.m_plans)
	{
		for (int i = average; i >= 0; i--)
			nearest[i] = 99999999999.0f;

		for (auto& plan2 : m_map.m_plans)
		{
			if (plan->getID() == plan2->getID())
				continue;
			for (int i = average - 1; i >= 0; i--)
			{
				float dist = (plan->getPos() - plan2->getPos()).length();
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
	return avg / m_map.m_nPlans;
}
