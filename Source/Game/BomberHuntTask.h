#pragma once
#include "Map.h"

class BomberHuntTask
{
public:
	BomberHuntTask(class PlayerAI& ai, MapEntity& bomber);
	void DoTask(float dt);
	void SetTarget(const MapEntity* target);
	int GetBomberID() const;
	const MapEntity* GetTarget() const;
public:
	static const int ID_START = 512;
private:
	class PlayerAI& ai;
	MapEntity& bomber;
	const MapEntity* pTarget;
	const int group;

	float timeout = 0.0f;
};