#pragma once
#include "MapPlanet.h"
#include "MapEntity.h"

class LocalPlanet : public MapPlanet
{
public:
	LocalPlanet(const MapLoader::MapPlanet& plan, unsigned int ID)
		:
		MapPlanet(plan,ID),
		// (radius / 10.0f) * Entity Damage per Second
		healRate((int)float(this->radius / 10.0f * (float(MapEntity::DAMAGE) / float(MapEntity::DAMAGE_INTV))))
	{}
	virtual ~LocalPlanet(){}
	virtual float GetsUnit() const override
	{
		return sUnit;
	}

	//returns true if entity shall be spawned
	virtual bool Update(const float dt) override
	{
		if (team == 0)
			return false;

		if (HP < MaxHP)
		{
			hTimer += dt;
			if (hTimer > 1.0f) // 1 sec
			{
				HP = std::min(MaxHP, HP + healRate);
				hTimer -= 1.0f;
			}
		}

		sTimer += dt;
		const float sTime = (sUnit * GetSpawnFactor());
		if (sTimer > sTime)
		{
			sTimer -= sTime;
			return true;
		}
		return false;
	}
	virtual void TakeOver(byte team) override
	{
		MapPlanet::TakeOver(team);
		sTimer = 0.0f;
		hTimer = 0.0f;
	}

	virtual float GetDrawPercentage() override
	{
		return lastPercentage;
	}
	virtual void CalcDrawPercentage() override
	{
		lastPercentage = GetPercentage() * 0.3f + lastPercentage * 0.7f;
	}
	virtual float GetSpawnTimePercent() override
	{
		float r = sTimer / (sUnit * GetSpawnFactor());
		return std::min(1.0f, r);
	}
private:
	float sTimer = 0.0f; // spawn timer
	float hTimer = 0.0f; // heal timer

	const int healRate;
	float lastPercentage = 1.0f;
};
