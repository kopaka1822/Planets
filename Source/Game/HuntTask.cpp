#include "HuntTask.h"
#include "PlayerAI.h"

HuntTask::HuntTask(PlayerAI& ai, MapEntity& startUnit)
	:
	ai(ai),
	id(ID_START + startUnit.getID()),
	pTarget(nullptr),
	curPos(startUnit.getPos()),
	groupSize(1)
{
	startUnit.groupAssign(id);
}
void HuntTask::DoTask(float dt)
{
	if (pTarget)
	{
		curPos = pTarget->getPos();
		ai.map.SelectGroup(ai.team, id);
		ai.map.Click(curPos, ai.team);
	}
	else
	{
		if (timeout <= 0.0f)
		{
			// search unit
			pTarget = ai.GetHuntTarget(curPos, curRadius);
			if (pTarget)
			{
				SetTarget(pTarget);
			}
			else
			{
				curRadius += RAD_STEP;
			}
			timeout += 2.0f;
		}

		timeout -= dt;
	}

}
void HuntTask::SetTarget(const MapEntity* target)
{
	pTarget = target;
	if (pTarget)
		curPos = pTarget->getPos();
}
