#pragma once
#include "MapPlanet.h"

class RemotePlanet : public MapPlanet
{
public:
	RemotePlanet(const MapLoader::MapPlanet& plan, unsigned int ID)
		:
		MapPlanet(plan,ID)
	{}
	virtual float GetDrawPercentage() override
	{	
		return lastHPPercent;
	}
	virtual void CalcDrawPercentage() override
	{
		lastHPPercent = GetPercentage() * 0.1f + lastHPPercent * 0.9f;
	}
	virtual ~RemotePlanet(){}
	virtual float GetSpawnTimePercent() override
	{
		float nw = GetSpawnPer();
		if (nw > lastSpawnPercent)
		{
			lastSpawnPercent = std::min(lastSpawnPercent * 0.7f + 0.3f * nw, 1.0f);
		}
		else
			lastSpawnPercent = nw;

		return lastSpawnPercent;
	}
	virtual bool Update(const float dt) override
	{
		sTimer += dt;

		return false;
	}
	virtual void ResetSpawnTime() override
	{
		sTimer = 0.0f;
	}
private:
	float GetSpawnPer() const
	{
		float r = sTimer / (sUnit * GetSpawnFactor());
		return r;
	}
private:
	float lastHPPercent = 1.0f;
	float sTimer = 0.0f;
	float lastSpawnPercent = 0.0f;
};