#pragma once
#include "../Utility/Rect.h"
#include "MapLoader.h"
#include "MapObject.h"

using byte = unsigned char;

class MapPlanet : public MapObject
{
public:
	MapPlanet(const MapLoader::MapPlanet& plan, GameID ID)
		:
		MapObject({ plan.x, plan.y }, plan.team, plan.HP,MapObject::tgPlanetDefend,PointF(ID,0.0f),-1),
        m_sUnit(plan.sUnit),
		m_radius(plan.radius),
		m_colRect(RectF(plan.x - plan.radius, plan.y - plan.radius, plan.x + plan.radius, plan.y + plan.radius)),
		m_r2(m_radius * m_radius),
		m_nearR(m_radius + PLAN_NEAR),
		m_nearR2( m_nearR * m_nearR),
		m_defR(m_radius + float(PLAN_DEFENSE)),
		m_defR2(m_defR * m_defR),
		m_maxHP(plan.HP),
        m_ID(PlanetID(ID)),
        m_curEntType(etNormal)
	{
	}
	virtual ~MapPlanet(){}

	float getRadius() const
	{
		return m_radius;
	}
	float getDefenseRadius() const
	{
		return m_defR;
	}
	virtual bool update(const float dt) = 0;
	inline bool isColliding(const PointF& p) const
	{
		/*if (fabs(p.x - pos.x) <= radius)
			if (fabs(p.y - pos.y) <= radius)
				if ((p - pos).lengthSq() <= r2)
					return true;*/
		if (m_colRect.PointInside(p))
		{
			if ((p - m_pos).lengthSq() <= m_r2)
			{
				return true;
			}
		}
		return false;
	}
	inline bool isNearby(const PointF& p) const
	{
		if (fabs(p.x - m_pos.x) <= m_nearR)
			if (fabs(p.y - m_pos.y) <= m_nearR)
				if ((p - m_pos).lengthSq() <= m_nearR2)
					return true;

		return false;
	}
	//inside defense radius
	inline bool isInDefend(const PointF& p) const
	{
		if (fabs(p.x - m_pos.x) <= m_defR)
			if (fabs(p.y - m_pos.y) <= m_defR)
				if ((p - m_pos).lengthSq() <= m_defR2)
					return true;

		return false;
	}
	bool takeDamage(GameHP amount, TeamID team) //returns true if captured
	{
		//if (this->m_team != m_team)
		//{
			if (this->m_team != 0) // someone owns this planet
			{
				m_hp -= amount;
				if (m_hp <= 0)
				{
					takeOver(team);
					return true;
				}
			}
			else
			{//noone owns this planet
				if (m_hp == m_maxHP)
				{
					m_subteam = team;
				}

				if (m_subteam == team)
				{
					m_hp -= amount;
					if (m_hp <= 0)
					{
						takeOver(team);
						return true;
					}
				}
				else  //subteam != m_team
				{
					m_hp += amount;
					if (m_hp >= m_maxHP)
					{
						m_hp = m_maxHP;
						m_subteam = 0;
					}
				}
			}
		//}
		return false;
	}
	int getMaxHP() const
	{
		return m_maxHP;
	}
	float getPercentage() const
	{
		return float(m_hp) / float(m_maxHP);
	}
	virtual float getDrawPercentage()
	{
		return getDrawPercentage();
	}
	virtual void calcDrawPercentage()
	{}
	short getID() const //-1 = invalid
	{
		return m_ID;
	}
	virtual void takeOver(TeamID team)
	{
		m_hp = m_maxHP;
		if (this->m_team == 0)
		{
			this->m_team = team;
		}
		else
		{
			this->m_team = 0;
		}
		m_subteam = 0;
		//reset some things..
		MapObject::deselect();
		MapObject::m_group = -1;
		MapObject::setTarget({ m_ID, 0.0f }, tgPlanetDefend);
		m_curEntType = etNormal;
	}
	virtual void sabotage(TeamID tea)
	{
		if (this->m_team == tea)
			return;

		if (this->m_team != 0)
		{
			takeOver(0);
		}
		else
		{
			takeOver(tea);
		}
	}
	void setEntityType(EntityType t)
	{
		m_curEntType = t;
	}
	EntityType getEntityType() const
	{
		return m_curEntType;
	}
	//Local
	virtual float getsUnit() const
	{
		return 0.0f;
	}
	TeamID getSubteam() const
	{
		return m_subteam;
	}
	int getSubHP(TeamID ownTeam) const
	{
		if (m_subteam)
		{
			if (m_subteam == ownTeam)
			{
				return m_hp;
			}
			else
			{
				return 2 * m_maxHP - m_hp;
			}
		}
		else
		{
			return 2 * m_hp;
		}
	}
	void setSubAndHP(TeamID sub, GameHP HP)
	{
		setHP(HP);
		m_subteam = sub;
	}

	virtual float getSpawnTimePercent() = 0;
	virtual void resetSpawnTime(){}
protected:
	float getSpawnFactor() const
	{
		//	              1
		//  f(x) =  --------------
		//			0.9 * x² + 0,1
		float perc = getPercentage();
		float y = 0.9f * perc * perc + 0.1f;
		return 1.0f / y * getEntSpawnFactor(getEntityType());
	}
protected:
	const float m_sUnit;
	const float m_radius;
	const RectF m_colRect;
	const float m_r2;
	const float m_nearR;
	const float m_nearR2;

	const float m_defR;
	const float m_defR2;

	const GameHP m_maxHP;

	const PlanetID m_ID;

	TeamID m_subteam = 0;

	EntityType m_curEntType;
public:
	static const int PLAN_DEFENSE = 20;
	static const int PLAN_NEAR = 15;
};
