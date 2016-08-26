#include "LocalMapSingleplayer.h"
#include "../Utility/Timer.h"
#include "PlayerAI.h"

LocalMapSingleplayer::LocalMapSingleplayer(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
	float width, float height, Map::GameType ty, std::vector<byte> clns, bool bInitAI)
	:
	LocalMap(nPlayers,planets,spawns,width,height,ty,clns)
{
	if (bInitAI)
		InitAI();
}
std::unique_ptr<LocalMapSingleplayer::AI> LocalMapSingleplayer::SpawnAI(int team)
{
	return std::unique_ptr<AI>(new PlayerAI(team,*this));
}
void LocalMapSingleplayer::InitAI()
{
	//make AIs
	for (int i = 1; i < nPlayers; i++)
	{
		pAI.push_back(SpawnAI(i + 1));
	}
	for (auto& a : pAI)
	{
		Map::addEventReciever(a.get());
	}
}
void LocalMapSingleplayer::Update(float dt)
{
	RefreshGrid();

	Timer t;
	t.StartWatch();
	for (auto& a : pAI)
	{
		a->DoTurn(dt);
	}

	for (auto& p : plans)
	{
		if (p->update(dt))
		{
			//spawn entity
			if (TryEntitySpawn(p->getPos(), p->getTeam(), p->getRadius(), p->getTargetType(), p->getTarget(), p->getGroup(), p->selected(), p->getDefenseRadius(), p->getEntityType()))
			{
				byte t = p->getTeam();
				Map::Event_EntitySpawn(p->getID(), *ents[t - 1].back());
				grid.AddEntity(&(*(ents[t - 1].back())), ents[t - 1].back()->getPos());
			}
		}
	}

	assert(dt > 0.0f);
	Map::ResetPlanDef();
	UpdateEnts(dt);
}