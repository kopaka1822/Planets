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
	for (int i = 1; i < m_nPlayers; i++)
	{
		pAI.push_back(SpawnAI(i + 1));
	}
	for (auto& a : pAI)
	{
		Map::addEventReciever(a.get());
	}
}
void LocalMapSingleplayer::update(float dt)
{
	refreshGrid();

	Timer t;
	t.StartWatch();
	for (auto& a : pAI)
	{
		a->doTurn(dt);
	}

	for (auto& p : m_plans)
	{
		if (p->update(dt))
		{
			//spawn entity
			if (tryEntitySpawn(p->getPos(), p->getTeam(), p->getRadius(), p->getTargetType(), p->getTarget(), p->getGroup(), p->selected(), p->getDefenseRadius(), p->getEntityType()))
			{
				byte t = p->getTeam();
				Map::Event_EntitySpawn(p->getID(), *m_ents[t - 1].back());
				m_grid.addEntity(&(*(m_ents[t - 1].back())), m_ents[t - 1].back()->getPos());
			}
		}
	}

	assert(dt > 0.0f);
	Map::resetPlanDef();
	updateEnts(dt);
}