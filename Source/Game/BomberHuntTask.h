#pragma once
#include "Map.h"

class BomberHuntTask
{
public:
	BomberHuntTask(class PlayerAI& ai, MapEntity& bomber);
	void doTask(float dt);
	void setTarget(const MapEntity* target);
	int getBomberID() const;
	const MapEntity* getTarget() const;
public:
	static const int ID_START = 512;
private:
	class PlayerAI& m_ai;
	MapEntity& m_bomber;
	const MapEntity* m_pTarget;
	const int m_group;

	float m_timeout = 0.0f;
};