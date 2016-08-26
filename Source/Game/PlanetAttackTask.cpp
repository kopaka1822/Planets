#include "PlanetAttackTask.h"
#include "PlayerAI.h"

PlanetAttackTask::PlanetAttackTask(PlayerAI& ai, PlanetID p)
	:
	m_ai(ai),
	m_plan(ai.m_map.getPlan(p)),
	m_gid(ID_START + p)
{

}
void PlanetAttackTask::destroy()
{
	m_ai.m_map.deleteGroup(m_ai.m_team,m_gid);
}
void PlanetAttackTask::update()
{
	m_ai.m_map.selectGroup(m_ai.m_team, m_gid);
	m_ai.m_map.setTarget(m_plan.getPos(), m_ai.m_team);
}
