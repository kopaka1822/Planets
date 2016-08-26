#pragma once
#include "MapPlanet.h"

class RemotePlanet : public MapPlanet
{
public:
	RemotePlanet(const MapLoader::MapPlanet& plan, unsigned int ID)
		:
		MapPlanet(plan,ID)
	{}
	virtual float getDrawPercentage() override
	{	
		return lastHPPercent;
	}
	virtual void calcDrawPercentage() override
	{
		lastHPPercent = getPercentage() * 0.1f + lastHPPercent * 0.9f;
	}
	virtual ~RemotePlanet(){}
	virtual float getSpawnTimePercent() override
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
	virtual bool update(const float dt) override
	{
		sTimer += dt;

		return false;
	}
	virtual void resetSpawnTime() override
	{
		sTimer = 0.0f;
	}
private:
	float GetSpawnPer() const
	{
		float r = sTimer / (m_sUnit * getSpawnFactor());
		return r;
	}
private:
	float lastHPPercent = 1.0f;
	float sTimer = 0.0f;
	float lastSpawnPercent = 0.0f;
};