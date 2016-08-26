#include "BomberHuntTask.h"
#include "PlayerAI.h"

BomberHuntTask::BomberHuntTask(PlayerAI& ai, MapEntity& bomber)
	:
	m_bomber(bomber),
	m_pTarget(nullptr),
	m_ai(ai),
	m_group(ID_START + bomber.getID())
{
	bomber.groupAssign(m_group);
}
void BomberHuntTask::doTask(float dt)
{
	if (m_pTarget)
	{
		m_ai.m_map.selectGroup(m_ai.m_team, m_group);
		m_ai.m_map.setTarget(m_pTarget->getPos(), m_ai.m_team);
	}
	else
	{
		if (m_timeout <= 0.0f)
		{
			// search target
			m_pTarget = m_ai.getBomberTarget(m_bomber);

			// timeout to avoid to much searching
			m_timeout = 2.0f;
		}

		m_timeout -= dt;
	}
}
void BomberHuntTask::setTarget(const MapEntity* target)
{
	m_pTarget = target;
}
int BomberHuntTask::getBomberID() const
{
	return m_bomber.getID();
}
const MapEntity* BomberHuntTask::getTarget() const
{
	return m_pTarget;
}