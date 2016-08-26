#pragma once
#include "../Utility/Rect.h"
#include <algorithm>
#include "MapObject.h"

#define EVADE_TIME 0.10f
#define TARGET_TIME 5.0f

class MapEntity : public MapObject
{
	//constants
public:
	static const int RADIUS;
	static const int RADIUS2;
	static const int MIN_DIST;
	static const int MIN_DIST2;

	static const int NEAR_DIST;
	static const int NEAR_DIST2;

	static const int SPEED;

	static const int DAMAGE;
	static const float DAMAGE_INTV;

	static const int MAX_CHARGES; //shall be used if the game is laggy

	static const int MAX_HP;

	static const int FOV; //Field of vision
	static const int FOV2;
public:
	enum States
	{
		sLazy,
		sTarget,
		sEvade
	};
	States GetState() const
	{
		return m_curState;
	}


public:
	MapEntity(const PointF& p, byte team, MapObject::TargetType ttype, const PointF& tar, int group, bool selec, unsigned int id)
		:
		MapObject(p, team, MAX_HP, ttype, tar, group),
		m_vel({ 0.0f, 0.0f }),
		m_id(id)
	{
		if (selec)
			MapObject::forceSelect();

		if (ttype == tgInvalid)
		{
			m_curState = sLazy;
		}
		else
		{
			m_curState = sTarget;
		}
		m_mood = false;
	}
	virtual ~MapEntity(){}

	virtual void calcDrawPos()
	{}
	virtual const PointF & getDrawPos()
	{
		return getPos();
	}
	const PointF& getVel() const
	{
		return m_vel;
	}

	inline bool isColliding(const PointF& p) const
	{
		if (fabs(m_pos.x - p.x) < float(MIN_DIST))
			if (fabs(m_pos.y - p.y) < float(MIN_DIST))
				return ((m_pos - p).lengthSq() < float(MIN_DIST2)); //5px
		return false;
		//return ((pos - p).lengthSq() < 25.0f); //5px
	}
	inline bool isNearby(const PointF& p) const
	{
		if (fabs(m_pos.x - p.x) < float(NEAR_DIST))
			if (fabs(m_pos.y - p.y) < float(NEAR_DIST))
				return ((m_pos - p).lengthSq() < float(NEAR_DIST2)); //5px
		return false;
		//return ((pos - p).lengthSq() < 225.0f); //15px
	}

	virtual void setTarget(const PointF& tg, TargetType tt) override
	{
		MapObject::setTarget(tg, tt);
		m_curState = sTarget;
		m_bTarReach = false;
	}
	virtual void invalidateTarget() override
	{
		MapObject::invalidateTarget();
		m_curState = sLazy;
	}

	virtual void updatePosition(const float dt)
	{
		m_pos += m_vel;

		if (m_curState == sEvade || (m_curState == sTarget && m_bTarReach))
		{
			m_Tmr -= dt;
			if (m_Tmr <= 0)
			{
				invalidateTarget();
				m_Tmr = 0.0f;
				return;
			}
			return;
		}
		return;
	}
	void setVel(PointF nVel)
	{
		m_vel = nVel;
	}
	void zeroVelocity()
	{
		m_vel = { 0.0f, 0.0f };
	}

	bool getMood() const
	{
		return m_mood;
	}
	void changeMood()
	{
		m_mood = !m_mood;
		if (m_curState == sEvade)//evading failed, evade in opposite direction
		{
			m_target *= -1.0f;
			return;
		}
		return;
	}
	void setEvade(PointF nTarget) //sets target + velocity
	{
		m_curState = sEvade;
		m_Tmr = (float)EVADE_TIME;
		m_target = nTarget.normalize();
		m_vel = nTarget;
	}
	void keepEvading()
	{
		m_Tmr = (float)EVADE_TIME;
	}
	void targetPointReached()
	{
		if (!m_bTarReach)
		{
			m_Tmr = TARGET_TIME;
			m_bTarReach = true;
			return;
		}
		return;
	}
	inline void disableVelCorrect()
	{
		m_velCorrect = false;
	}
	inline bool getVelCorrect()
	{
		bool t = m_velCorrect;
		m_velCorrect = true;
		return t;
	}
	virtual EntityType getEntityType() const = 0;

	//Remote
	virtual void setUpdateTime(float secon){}
	virtual float getUpdateTime() const { return 0.0f; }
	//Local
	virtual int getDamage(){ return 0; }
	virtual bool hasDamage() const{ return false; }
	inline unsigned int getID() const
	{
		return m_id;
	}
	virtual float getSpeedModifier() const
	{
		return 1.0f;
	}
	virtual bool attacksPlanets() const
	{
		return true;
	}
	virtual bool attacksEntities() const
	{
		return true;
	}

	// 0.0f -> no explosion on death
	virtual float getExplosionRadius() const
	{
		return 0.0f;
	}
	virtual int getExplosionDamage() const
	{
		return MAX_HP;
	}
protected:

	PointF m_vel;

	//PointF target;
	//targetType tarTy = tgPlanet;
	States m_curState = sLazy;

	bool m_mood; //current mood of the entity, determines if it will evade to the left or right
	bool m_bTarReach = false;

	float m_Tmr = 0.0f;


	bool m_velCorrect = true;
	const unsigned int m_id;
};
