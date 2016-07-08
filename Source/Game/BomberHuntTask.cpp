#include "BomberHuntTask.h"
#include "PlayerAI.h"

BomberHuntTask::BomberHuntTask(PlayerAI& ai, MapEntity& bomber)
	:
	bomber(bomber),
	pTarget(nullptr),
	ai(ai),
	group(ID_START + bomber.GetID())
{
	bomber.groupAssign(group);
}
void BomberHuntTask::DoTask(float dt)
{
	if (pTarget)
	{
		ai.map.SelectGroup(ai.team, group);
		ai.map.Click(pTarget->GetPos(), ai.team);
	}
	else
	{
		if (timeout <= 0.0f)
		{
			// search target
			pTarget = ai.GetBomberTarget(bomber);

			// timeout to avoid to much searching
			timeout = 2.0f;
		}

		timeout -= dt;
	}
}
void BomberHuntTask::SetTarget(const MapEntity* target)
{
	pTarget = target;
}
int BomberHuntTask::GetBomberID() const
{
	return bomber.GetID();
}
const MapEntity* BomberHuntTask::GetTarget() const
{
	return pTarget;
}