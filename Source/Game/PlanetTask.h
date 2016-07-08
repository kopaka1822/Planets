#pragma once
#include "Map.h"

class PlanetTask
{
public:
	PlanetTask(class PlayerAI& ai, PlanetID pid);
	PlanetTask& operator=(const PlanetTask&) = delete;
	void DoTask(float dt);
	void IncGroupSize(MapObject::entityType et)
	{
		if (et != MapObject::etNormal)
		{
			plan.SetEntityType(MapObject::etNormal);
			PointF tar;
			tar.x = (float)plan.GetID();
			plan.setTarget(tar, MapObject::tgPlanetDefend);
		}
		groupSize++;
		bEvent = true;
	}
	void DecGroupSize()
	{
		groupSize--;
		bEvent = true;
	}
	void AddToGroup(int s)
	{
		groupSize += s;
		bEvent = true;
	}
	void Destroy();
	// reselect untis and assign standart task
	void Update();
	bool isInTrouble() const
	{
		return bInTrouble;
	}
	size_t GetGroupSize()
	{
		return groupSize;
	}
public:
	static const int ID_START = 0;
	static const int ID_END = 255;
private:
	static const int MAX_ATTACK_SIZE = 20;
	static const int MAX_DEF_SIZE = 40;
private:
	PlanetID GetAttackPlan() const;
	PlanetID GetNearestFreePlan() const;
	size_t GetSplitSize() const;
	int GetAttackSize(PlanetID pid) const;
private:
	class PlayerAI& ai;
	MapPlanet& plan;
	size_t groupSize = 0;
	bool bInTrouble = false;

	float sumTime = 0.0f;
	bool bEvent = false;
};