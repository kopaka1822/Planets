#include "PlanetTask.h"
#include "PlayerAI.h"

PlanetTask::PlanetTask(PlayerAI& ai, PlanetID pid)
	:
	ai(ai),
	plan(ai.map.getPlan(pid))
{
	plan.groupAssign(pid); // 0 is a valid group
}
void PlanetTask::DoTask(float dt)
{
	sumTime += dt;

	if (!bEvent && sumTime < 2.0f)
		return; // sleep

	sumTime = 0.0f;

	bInTrouble = (plan.GetPercentage() < 0.95f && groupSize < 10) || (plan.GetPercentage() < 0.85f && groupSize < 20);
	if (!bInTrouble)
	{
		// not in trouble
		unsigned int enemiesNear = ai.GetEnemyEntsInR(plan.GetPos(), ai.team, plan.GetDefenseRadius() * 4);
		unsigned int ownEntsNear = ai.GetOwnEntsInR(plan.GetPos(), ai.team, plan.GetDefenseRadius() * 3);
		if (enemiesNear < ownEntsNear)
		{
			// less enemies nearby
			PlanetID help = ai.GetPlanetInTrouble(plan.GetGroup());
			if (help != plan.GetID())
			{
				// other planet needs help
				size_t num = ai.HelpPlanet(plan.GetGroup(), groupSize * 2 / 3, help);
				groupSize -= num;
			}
			else
			{
				// no other planet needs help

				if (ai.GetEnemyEntsInR(plan.GetPos(), ai.team, plan.GetDefenseRadius() * 3) <= 0 && ai.GetWhitePlan(plan.GetID()) != plan.GetID() && ai.GetEnemyEntsInR(ai.map.getPlan(ai.GetWhitePlan(plan.GetID())).GetPos(), ai.team, ai.map.getPlan(ai.GetWhitePlan(plan.GetID())).GetDefenseRadius() * 2) <= 0)
				{
					plan.SetEntityType(MapObject::etSaboteur);
					PointF tar;
					tar.x = (float)plan.GetID();
					plan.setTarget(tar, MapObject::tgPlanetDefend);
				}
				else
				{
					PlanetID pid = GetAttackPlan();

					if (pid != plan.GetID())
					{
						size_t attacksize = GetAttackSize(pid);
						if (attacksize <= groupSize)
						{
							// attack plan
							auto num = ai.AttackPlanet(plan.GetGroup(), attacksize, pid);
							groupSize -= num;
						}
						else
						{
							// wait for more unist
							plan.SetEntityType(MapObject::etNormal);
							PointF tar;
							tar.x = (float)plan.GetID();
							plan.setTarget(tar, MapObject::tgPlanetDefend);
						}
					}
					else
					{
						// you have all planets ->  hunt down remaining
						plan.SetEntityType(MapObject::etSpeeder);
					}

				}
			}
		}
		else
		{
			// many enemies nearby
			if (ai.GetEnemyAccumulation(plan.GetPos(), 250.0f, 8) != PointI(-1, -1))
			{
				// enemy accumulation nearby
				plan.SetEntityType(MapObject::etBomber);
				PointF tar;
				tar.x = (float)plan.GetID();
				plan.setTarget(tar, MapObject::tgPlanetDefend);
			}
			else
			{
				// no enemy accumulation nearby
				plan.SetEntityType(MapObject::etNormal);
				PointF tar;
				tar.x = (float)plan.GetID();
				plan.setTarget(tar, MapObject::tgPlanetDefend);
				if (ai.GetAlliEntsInDefR(plan.GetID(), ai.team) > MAX_DEF_SIZE)
				{
					PlanetID pid = GetAttackPlan();
					size_t attacksize = GetAttackSize(pid);
					if (attacksize * 2 <= groupSize && pid != plan.GetID())
						ai.AttackPlanet(plan.GetGroup(), attacksize, pid);
				}
			}
		}

	}
	else
	{
		// in trouble
		if (ai.GetEnemyEntsInDefR(plan.GetID(), ai.team) > 10)
			plan.SetEntityType(MapObject::etBomber);
		else
			plan.SetEntityType(MapObject::etNormal);

		PointF tar;
		tar.x = (float)plan.GetID();
		plan.setTarget(tar, MapObject::tgPlanetDefend);
	}


	bEvent = false;
}
void PlanetTask::Destroy()
{
	//ai.map.DeleteGroup(ai.team, plan.GetID());
	// Attack Planet to get it back
	ai.AttackPlanet(plan.GetID(), groupSize, plan.GetID());
}
PlanetID PlanetTask::GetAttackPlan() const
{
	PlanetID fav = plan.GetID();

	float lastDist = std::numeric_limits<float>::infinity();
	for (const auto& p : ai.map.plans)
	{
		if (!ai.map.isAlly(p->GetTeam(), ai.team))
		{
			if (ai.GetAlliEntsInDefR(p->GetID(), ai.team) > 30 && ai.GetEnemyEntsInDefR(p->GetID(), ai.team) < 5)
				continue;
			p->GetSpawnTimePercent();
			float perc = p->GetPercentage();
			float y = 0.9f * perc * perc + 0.1f;
			float d =
				(plan.GetPos() - p->GetPos()).lengthSq()
				* (ai.GetEnemyEntsInDefR(p->GetID(), ai.team) + y * 10)
				+ rand() % 10;
			if (d < lastDist)
			{
				lastDist = d;
				fav = p->GetID();
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
		if (!ai.map.isAlly(p->GetTeam(), ai.team))
		{
			float d = (plan.GetPos() - p->GetPos()).lengthSq();
			if (d < lastDist)
			{
				lastDist = d;
				fav = p->GetID();
			}
		}
	}

	return fav;
}
void PlanetTask::Update()
{
	ai.map.SelectGroup(ai.team, plan.GetID());
	ai.map.Click(plan.GetPos(), ai.team);
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
