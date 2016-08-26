#include "PlanetTask.h"
#include "PlayerAI.h"

PlanetTask::PlanetTask(PlayerAI& ai, PlanetID pid)
	:
	m_ai(ai),
	m_plan(ai.m_map.getPlan(pid))
{
	m_plan.groupAssign(pid); // 0 is a valid m_group
}
void PlanetTask::doTask(float dt)
{
	m_sumTime += dt;

	if (!m_bEvent && m_sumTime < 2.0f)
		return; // sleep

	m_sumTime = 0.0f;

	m_bInTrouble = (m_plan.getPercentage() < 0.95f && m_groupSize < 10) || (m_plan.getPercentage() < 0.85f && m_groupSize < 20);
	if (!m_bInTrouble)
	{
		// not in trouble
		unsigned int enemiesNear = m_ai.getEnemyEntsInR(m_plan.getPos(), m_ai.m_team, m_plan.getDefenseRadius() * 4);
		unsigned int ownEntsNear = m_ai.getOwnEntsInR(m_plan.getPos(), m_ai.m_team, m_plan.getDefenseRadius() * 3);
		if (enemiesNear < ownEntsNear)
		{
			// less enemies nearby
			PlanetID help = m_ai.getPlanetInTrouble(m_plan.getGroup());
			if (help != m_plan.getID())
			{
				// other planet needs help
				size_t num = m_ai.helpPlanet(m_plan.getGroup(), m_groupSize * 2 / 3, help);
				m_groupSize -= num;
			}
			else
			{
				// no other planet needs help

				if (m_ai.getEnemyEntsInR(m_plan.getPos(), m_ai.m_team, m_plan.getDefenseRadius() * 3) <= 0 && m_ai.getWhitePlan(m_plan.getID()) != m_plan.getID() && m_ai.getEnemyEntsInR(m_ai.m_map.getPlan(m_ai.getWhitePlan(m_plan.getID())).getPos(), m_ai.m_team, m_ai.m_map.getPlan(m_ai.getWhitePlan(m_plan.getID())).getDefenseRadius() * 2) <= 0)
				{
					m_plan.setEntityType(MapObject::etSaboteur);
					PointF tar;
					tar.x = (float)m_plan.getID();
					m_plan.setTarget(tar, MapObject::tgPlanetDefend);
				}
				else
				{
					PlanetID pid = getAttackPlan();

					if (pid != m_plan.getID())
					{
						size_t attacksize = getAttackSize(pid);
						if (attacksize <= m_groupSize)
						{
							// attack plan
							auto num = m_ai.attackPlanet(m_plan.getGroup(), attacksize, pid);
							m_groupSize -= num;
						}
						else
						{
							// wait for more unist
							m_plan.setEntityType(MapObject::etNormal);
							PointF tar;
							tar.x = (float)m_plan.getID();
							m_plan.setTarget(tar, MapObject::tgPlanetDefend);
						}
					}
					else
					{
						// you have all planets ->  hunt down remaining
						m_plan.setEntityType(MapObject::etSpeeder);
					}

				}
			}
		}
		else
		{
			// many enemies nearby
			if (m_ai.getEnemyAccumulation(m_plan.getPos(), 250.0f, 8) != PointI(-1, -1))
			{
				// enemy accumulation nearby
				m_plan.setEntityType(MapObject::etBomber);
				PointF tar;
				tar.x = (float)m_plan.getID();
				m_plan.setTarget(tar, MapObject::tgPlanetDefend);
			}
			else
			{
				// no enemy accumulation nearby
				m_plan.setEntityType(MapObject::etNormal);
				PointF tar;
				tar.x = (float)m_plan.getID();
				m_plan.setTarget(tar, MapObject::tgPlanetDefend);
				if (m_ai.getAlliEntsInDefR(m_plan.getID(), m_ai.m_team) > MAX_DEF_SIZE)
				{
					PlanetID pid = getAttackPlan();
					size_t attacksize = getAttackSize(pid);
					if (attacksize * 2 <= m_groupSize && pid != m_plan.getID())
						m_ai.attackPlanet(m_plan.getGroup(), attacksize, pid);
				}
			}
		}

	}
	else
	{
		// in trouble
		if (m_ai.getEnemyEntsInDefR(m_plan.getID(), m_ai.m_team) > 10)
			m_plan.setEntityType(MapObject::etBomber);
		else
			m_plan.setEntityType(MapObject::etNormal);

		PointF tar;
		tar.x = (float)m_plan.getID();
		m_plan.setTarget(tar, MapObject::tgPlanetDefend);
	}


	m_bEvent = false;
}
void PlanetTask::destroy()
{
	//ai.map.DeleteGroup(ai.m_team, plan.GetID());
	// Attack Planet to get it back
	m_ai.attackPlanet(m_plan.getID(), m_groupSize, m_plan.getID());
}
PlanetID PlanetTask::getAttackPlan() const
{
	PlanetID fav = m_plan.getID();

	float lastDist = std::numeric_limits<float>::infinity();
	for (const auto& p : m_ai.m_map.m_plans)
	{
		if (!m_ai.m_map.isAlly(p->getTeam(), m_ai.m_team))
		{
			if (m_ai.getAlliEntsInDefR(p->getID(), m_ai.m_team) > 30 && m_ai.getEnemyEntsInDefR(p->getID(), m_ai.m_team) < 5)
				continue;
			p->getSpawnTimePercent();
			float perc = p->getPercentage();
			float y = 0.9f * perc * perc + 0.1f;
			float d =
				(m_plan.getPos() - p->getPos()).lengthSq()
				* (m_ai.getEnemyEntsInDefR(p->getID(), m_ai.m_team) + y * 10)
				+ rand() % 10;
			if (d < lastDist)
			{
				lastDist = d;
				fav = p->getID();
			}
		}
	}

	return fav;
}
PlanetID PlanetTask::getNearestFreePlan() const
{
	PlanetID fav = 0;

	float lastDist = std::numeric_limits<float>::infinity();
	for (const auto& p : m_ai.m_map.m_plans)
	{
		if (!m_ai.m_map.isAlly(p->getTeam(), m_ai.m_team))
		{
			float d = (m_plan.getPos() - p->getPos()).lengthSq();
			if (d < lastDist)
			{
				lastDist = d;
				fav = p->getID();
			}
		}
	}

	return fav;
}
void PlanetTask::update()
{
	m_ai.m_map.selectGroup(m_ai.m_team, m_plan.getID());
	m_ai.m_map.setTarget(m_plan.getPos(), m_ai.m_team);
}
size_t PlanetTask::getSplitSize() const
{
	if (m_ai.countPlanets())
		if (m_ai.countUnits() < 15)
			return 0;
		else
			return std::min(m_ai.countUnits() / m_ai.countPlanets() / 3,size_t(30));
	else
		return 0;
}
int PlanetTask::getAttackSize(PlanetID pid) const
{
	int tmp = m_ai.getEnemyEntsInDefR(pid, m_ai.m_team);
	if (tmp > MAX_ATTACK_SIZE)
		return MAX_ATTACK_SIZE;
	else
		return tmp;
}
