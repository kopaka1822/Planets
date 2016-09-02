#include "LocalMapClassicMode.h"
#include <float.h>

LocalMapClassicMode::LocalMapClassicMode(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
	float width, float height, Map::GameType ty, std::vector<TeamID> clns)
	:
	LocalMapSingleplayer(nPlayers,planets,spawns,width,height,ty,clns,false)
{
	InitAI();
}

std::unique_ptr<LocalMapSingleplayer::AI> LocalMapClassicMode::SpawnAI(int team)
{
	return std::unique_ptr<AI>(new WeakAI(team, *this));
}

WeakAI::WeakAI(int team, Map& map)
	:
	m_myTeam(team),
	m_map(map)
{

}

void WeakAI::doTurn(float dt)
{
	m_sumTime += dt;

	if (m_sumTime > 3.0f)
	{
		m_sumTime = 0.0f;
		// select all and send on nearest target
		PointF tar = GetTarget();
		m_map.selectAll(m_myTeam);
		m_map.setTarget(tar, m_myTeam);
	}
}

PointF WeakAI::GetTarget()
{
	int nPlans = 0;
	PointF mid;
	for (const auto& p : m_map.m_plans)
	{
		if (p->getTeam() == m_myTeam)
		{
			nPlans++;
			mid += p->getPos();
		}
	}

	if (nPlans)
	{
		// we have planets
		if (nPlans != m_map.m_nPlans)
		{
			// a planet should be free
			return GetNearestPlan(mid);
		}
		else
		{
			// we have all planets attack entities
			return GetNearestEnt();
		}
	}
	else
	{
		// we dont have planets -> attack first planet
		if (m_map.m_nPlans)
		{
			return m_map.m_plans[0]->getPos();
		}
		else
		{
			return GetNearestEnt();
		}
	}
}
PointF WeakAI::GetNearestPlan(PointF pos)
{
	PointF nearesPos;
	float lastDist = FLT_MAX;
	bool bFoundWhite = false;

	for (const auto& p : m_map.m_plans)
	{
		if (p->getTeam() == 0)
		{
			float dist = (pos - p->getPos()).lengthSq();
			if (dist < lastDist)
			{
				lastDist = dist;
				nearesPos = p->getPos();
				bFoundWhite = true;
			}
		}
	}
	if (bFoundWhite)
		return nearesPos;

	lastDist = FLT_MAX;

	for (const auto& p : m_map.m_plans)
	{
		if (p->getTeam() != m_myTeam)
		{
			float dist = (pos - p->getPos()).lengthSq();
			if (dist < lastDist)
			{
				lastDist = dist;
				nearesPos = p->getPos();
			}
		}
	}

	return nearesPos;
}
PointF WeakAI::GetNearestEnt()
{
	for (size_t i = 0; i < m_map.m_nPlayers; i++)
	{
		if (i != m_myTeam)
		{
			if (m_map.m_ents[i].length())
			{
				return m_map.m_ents[i].begin()->getPos();
			}
		}
	}

	return PointF(m_map.m_mWidth, m_map.m_mHeight) * 0.5f;
}
