#include "LocalMapClassicMode.h"
#include <float.h>

LocalMapClassicMode::LocalMapClassicMode(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
	float width, float height, Map::GameType ty, std::vector<byte> clns)
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
	myTeam(team),
	map(map)
{

}

void WeakAI::DoTurn(float dt)
{
	sumTime += dt;

	if (sumTime > 3.0f)
	{
		sumTime = 0.0f;
		// select all and send on nearest target
		PointF tar = GetTarget();
		map.SelectAll(myTeam);
		map.Click(tar, myTeam);
	}
}

PointF WeakAI::GetTarget()
{
	int nPlans = 0;
	PointF mid;
	for (const auto& p : map.plans)
	{
		if (p->GetTeam() == myTeam)
		{
			nPlans++;
			mid += p->GetPos();
		}
	}

	if (nPlans)
	{
		// we have planets
		if (nPlans != map.nPlans)
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
		if (map.nPlans)
		{
			return map.plans[0]->GetPos();
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

	for (const auto& p : map.plans)
	{
		if (p->GetTeam() == 0)
		{
			float dist = (pos - p->GetPos()).lengthSq();
			if (dist < lastDist)
			{
				lastDist = dist;
				nearesPos = p->GetPos();
				bFoundWhite = true;
			}
		}
	}
	if (bFoundWhite)
		return nearesPos;

	lastDist = FLT_MAX;

	for (const auto& p : map.plans)
	{
		if (p->GetTeam() != myTeam)
		{
			float dist = (pos - p->GetPos()).lengthSq();
			if (dist < lastDist)
			{
				lastDist = dist;
				nearesPos = p->GetPos();
			}
		}
	}

	return nearesPos;
}
PointF WeakAI::GetNearestEnt()
{
	for (int i = 0; i < map.nPlayers; i++)
	{
		if (i != myTeam)
		{
			if (map.ents[i].length())
			{
				return map.ents[i].begin()->GetPos();
			}
		}
	}

	return PointF(map.mWidth, map.mHeight) * 0.5f;
}
