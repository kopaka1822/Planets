#include "HuntTask.h"
#include "PlayerAI.h"

HuntTask::HuntTask(PlayerAI& ai, MapEntity& startUnit)
	:
	m_ai(ai),
	m_id(ID_START + startUnit.getID()),
	m_pTarget(nullptr),
	m_curPos(startUnit.getPos()),
	m_groupSize(1)
{
	startUnit.groupAssign(m_id);
}
void HuntTask::doTask(float dt)
{
	if (m_pTarget)
	{
		m_curPos = m_pTarget->getPos();
		m_ai.m_map.selectGroup(m_ai.m_team, m_id);
		m_ai.m_map.setTarget(m_curPos, m_ai.m_team);
	}
	else
	{
		if (m_timeout <= 0.0f)
		{
			// search unit
			m_pTarget = m_ai.getHuntTarget(m_curPos, m_curRadius);
			if (m_pTarget)
			{
				setTarget(m_pTarget);
			}
			else
			{
				m_curRadius += RAD_STEP;
			}
			m_timeout += 2.0f;
		}

		m_timeout -= dt;
	}

}
void HuntTask::setTarget(const MapEntity* target)
{
	m_pTarget = target;
	if (m_pTarget)
		m_curPos = m_pTarget->getPos();
}
