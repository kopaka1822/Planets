#include "PlanetTask.h"
#include "PlayerAI.h"

PlanetTask::PlanetTask(PlayerAI& ai, PlanetID pid)
	:
	ai(ai),
	plan(ai.map.getPlan(pid))
{
	plan.groupAssign(pid); // 0 is a valid m_group
}
void PlanetTask::DoTask(float dt)
{
	sumTime += dt;

	if (!bEvent && sumTime < 2.0f)
		return; // sleep

	sumTime = 0.0f;

	bInTrouble = (plan.getPercentage() < 0.95f && groupSize < 10) || (plan.getPercentage() < 0.85f && groupSize < 20);
	if (!bInTrouble)
	{
		// not in trouble
		unsigned int enemiesNear = ai.GetEnemyEntsInR(plan.getPos(), ai.team, plan.getDefenseRadius() * 4);
		unsigned int ownEntsNear = ai.GetOwnEntsInR(plan.getPos(), ai.team, plan.getDefenseRadius() * 3);
		if (enemiesNear < ownEntsNear)
		{
			// less enemies nearby
			PlanetID help = ai.GetPlanetInTrouble(plan.getGroup());
			if (help != plan.getID())
			{
				// other planet needs help
				size_t num = ai.HelpPlanet(plan.getGroup(), groupSize * 2 / 3, help);
				groupSize -= num;
			}
			else
			{
				// no other planet needs help

				if (ai.GetEnemyEntsInR(plan.getPos(), ai.team, plan.getDefenseRadius() * 3) <= 0 && ai.GetWhitePlan(plan.getID()) != plan.getID() && ai.GetEnemyEntsInR(ai.map.getPlan(ai.GetWhitePlan(plan.getID())).getPos(), ai.team, ai.map.getPlan(ai.GetWhitePlan(plan.getID())).getDefenseRadius() * 2) <= 0)
				{
					plan.setEntityType(MapObject::etSaboteur);
					PointF tar;
					tar.x = (float)plan.getID();
					plan.setTarget(tar, MapObject::tgPlanetDefend);
				}
				else
				{
					PlanetID pid = GetAttackPlan();

					if (pid != plan.getID())
					{
						size_t attacksize = GetAttackSize(pid);
						if (attacksize <= groupSize)
						{
							// attack plan
							auto num = ai.AttackPlanet(plan.getGroup(), attacksize, pid);
							groupSize -= num;
						}
						else
						{
							// wait for more unist
							plan.setEntityType(MapObject::etNormal);
							PointF tar;
							tar.x = (float)plan.getID();
							plan.setTarget(tar, MapObject::tgPlanetDefend);
						}
					}
					else
					{
						// you have all planets ->  hunt down remaining
						plan.setEntityType(MapObject::etSpeeder);
					}

				}
			}
		}
		else
		{
			// many enemies nearby
			if (ai.GetEnemyAccumulation(plan.getPos(), 250.0f, 8) != PointI(-1, -1))
			{
				// enemy accumulation nearby
				plan.setEntityType(MapObject::etBomber);
				PointF tar;
				tar.x = (float)plan.getID();
				plan.setTarget(tar, MapObject::tgPlanetDefend);
			}
			else
			{
				// no enemy accumulation nearby
				plan.setEntityType(MapObject::etNormal);
				PointF tar;
				tar.x = (float)plan.getID();
				plan.setTarget(tar, MapObject::tgPlanetDefend);
				if (ai.GetAlliEntsInDefR(plan.getID(), ai.team) > MAX_DEF_SIZE)
				{
					PlanetID pid = GetAttackPlan();
					size_t attacksize = GetAttackSize(pid);
					if (attacksize * 2 <= groupSize && pid != plan.getID())
						ai.AttackPlanet(plan.getGroup(), attacksize, pid);
				}
			}
		}

	}
	else
	{
		// in trouble
		if (ai.GetEnemyEntsInDefR(plan.getID(), ai.team) > 10)
			plan.setEntityType(MapObject::etBomber);
		else
			plan.setEntityType(MapObject::etNormal);

		PointF tar;
		tar.x = (float)plan.getID();
		plan.setTarget(tar, MapObject::tgPlanetDefend);
	}


	bEvent = false;
}
void PlanetTask::Destroy()
{
	//ai.map.DeleteGroup(ai.m_team, plan.GetID());
	// Attack Planet to get it back
	ai.AttackPlanet(plan.getID(), groupSize, plan.getID());
}
PlanetID PlanetTask::GetAttackPlan() const
{
	PlanetID fav = plan.getID();

	float lastDist = std::numeric_limits<float>::infinity();
	for (const auto& p : ai.map.plans)
	{
		if (!ai.map.isAlly(p->getTeam(), ai.team))
		{
			if (ai.GetAlliEntsInDefR(p->getID(), ai.team) > 30 && ai.GetEnemyEntsInDefR(p->getID(), ai.team) < 5)
				continue;
			p->getSpawnTimePercent();
			float perc = p->getPercentage();
			float y = 0.9f * perc * perc + 0.1f;
			float d =
				(plan.getPos() - p->getPos()).lengthSq()
				* (ai.GetEnemyEntsInDefR(p->getID(), ai.team) + y * 10)
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
PlanetID PlanetTask::GetNearestFreePlan() const
{
	PlanetID fav = 0;

	float lastDist = std::numeric_limits<float>::infinity();
	for (const auto& p : ai.map.plans)
	{
		if (!ai.map.isAlly(p->getTeam(), ai.team))
		{
			float d = (plan.getPos() - p->getPos()).lengthSq();
			if (d < lastDist)
			{
				lastDist = d;
				fav = p->getID();
			}
		}
	}

	return fav;
}
void PlanetTask::Update()
{
	ai.map.SelectGroup(ai.team, plan.getID());
	ai.map.Click(plan.getPos(), ai.team);
}
size_t PlanetTask::GetSplitSize() const
{
	if (ai.CountPlanets())
		if (ai.CountUnits() < 15)
			return 0;
		else
			return std::min(ai.CountUnits() / ai.CountPlanets() / 3,size_t(30));
	else
		return 0;
}
int PlanetTask::GetAttackSize(PlanetID pid) const
{
	int tmp = ai.GetEnemyEntsInDefR(pid, ai.team);
	if (tmp > MAX_ATTACK_SIZE)
		return MAX_ATTACK_SIZE;
	else
		return tmp;
}
