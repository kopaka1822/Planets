#include "PlanetAttackTask.h"
#include "PlayerAI.h"

PlanetAttackTask::PlanetAttackTask(PlayerAI& ai, PlanetID p)
	:
	ai(ai),
	plan(ai.map.getPlan(p)),
	gid(ID_START + p)
{

}
void PlanetAttackTask::Destroy()
{
	ai.map.DeleteGroup(ai.team,gid);
}
void PlanetAttackTask::Update()
{
	ai.map.SelectGroup(ai.team, gid);
	ai.map.Click(plan.getPos(), ai.team);
}
