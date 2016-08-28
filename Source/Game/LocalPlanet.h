#pragma once
#include "MapPlanet.h"
#include "MapEntity.h"

class LocalPlanet : public MapPlanet
{
public:
	LocalPlanet(const MapLoader::MapPlanet& plan, GameID ID)
		:
		MapPlanet(plan,ID),
		// (radius / 10.0f) * Entity Damage per Second
		m_healRate(static_cast<int>(float(this->m_radius / 10.0f * (float(MapEntity::DAMAGE) / float(MapEntity::DAMAGE_INTV)))))
	{}
	virtual ~LocalPlanet(){}
	virtual float getsUnit() const override
	{
		return m_sUnit;
	}

	//returns true if entity shall be spawned
	virtual bool update(const float dt) override
	{
		if (m_team == 0)
			return false;

		if (m_hp < m_maxHP)
		{
			m_hTimer += dt;
			if (m_hTimer > 1.0f) // 1 sec
			{
				m_hp = std::min(m_maxHP, m_hp + m_healRate);
				m_hTimer -= 1.0f;
			}
		}

		m_sTimer += dt;
		const float sTime = (m_sUnit * getSpawnFactor());
		if (m_sTimer > sTime)
		{
			m_sTimer -= sTime;
			return true;
		}
		return false;
	}
	virtual void takeOver(TeamID team) override
	{
		MapPlanet::takeOver(team);
		m_sTimer = 0.0f;
		m_hTimer = 0.0f;
	}

	virtual float getDrawPercentage() override
	{
		return m_lastPercentage;
	}
	virtual void calcDrawPercentage() override
	{
		m_lastPercentage = getPercentage() * 0.3f + m_lastPercentage * 0.7f;
	}
	virtual float getSpawnTimePercent() override
	{
		float r = m_sTimer / (m_sUnit * getSpawnFactor());
		return std::min(1.0f, r);
	}
private:
	float m_sTimer = 0.0f; // spawn timer
	float m_hTimer = 0.0f; // heal timer

	const GameHP m_healRate;
	float m_lastPercentage = 1.0f;
};
