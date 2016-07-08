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
	enum states
	{
		sLazy,
		sTarget,
		sEvade
	};
	states GetState() const
	{
		return curState;
	}


public:
	MapEntity(const PointF& p, byte team, MapObject::targetType ttype, const PointF& tar, int group, bool selec, unsigned int id)
		:
		MapObject(p, team, MAX_HP, ttype, tar, group),
		vel({ 0.0f, 0.0f }),
		id(id)
	{
		if (selec)
			MapObject::forceSelect();

		if (ttype == tgInvalid)
		{
			curState = sLazy;
		}
		else
		{
			curState = sTarget;
		}
		mood = false;
	}
	virtual ~MapEntity(){}

	virtual void CalcDrawPos()
	{}
	virtual const PointF GetDrawPos()
	{
		return GetPos();
	}
	const PointF& GetVel() const
	{
		return vel;
	}

	inline bool isColliding(const PointF& p) const
	{
		if (fabs(pos.x - p.x) < float(MIN_DIST))
			if (fabs(pos.y - p.y) < float(MIN_DIST))
				return ((pos - p).lengthSq() < float(MIN_DIST2)); //5px
		return false;
		//return ((pos - p).lengthSq() < 25.0f); //5px
	}
	inline bool isNearby(const PointF& p) const
	{
		if (fabs(pos.x - p.x) < float(NEAR_DIST))
			if (fabs(pos.y - p.y) < float(NEAR_DIST))
				return ((pos - p).lengthSq() < float(NEAR_DIST2)); //5px
		return false;
		//return ((pos - p).lengthSq() < 225.0f); //15px
	}

	virtual void setTarget(const PointF& tg, targetType tt) override
	{
		MapObject::setTarget(tg, tt);
		curState = sTarget;
		bTarReach = false;
	}
	virtual void invalidateTarget() override
	{
		MapObject::invalidateTarget();
		curState = sLazy;
	}

	virtual void UpdatePosition(const float dt)
	{
		pos += vel;

		if (curState == sEvade || (curState == sTarget && bTarReach))
		{
			Tmr -= dt;
			if (Tmr <= 0)
			{
				invalidateTarget();
				Tmr = 0.0f;
				return;
			}
			return;
		}
		return;
	}
	void SetVel(PointF nVel)
	{
		vel = nVel;
	}
	void ZeroVelocity()
	{
		vel = { 0.0f, 0.0f };
	}

	bool GetMood() const
	{
		return mood;
	}
	void ChangeMood()
	{
		mood = !mood;
		if (curState == sEvade)//evading failed, evade in opposite direction
		{
			target *= -1.0f;
			return;
		}
		return;
	}
	void setEvade(PointF nTarget) //sets target + velocity
	{
		curState = sEvade;
		Tmr = (float)EVADE_TIME;
		target = nTarget.normalize();
		vel = nTarget;
	}
	void keepEvading()
	{
		Tmr = (float)EVADE_TIME;
	}
	void TargetPointReached()
	{
		if (!bTarReach)
		{
			Tmr = TARGET_TIME;
			bTarReach = true;
			return;
		}
		return;
	}
	inline void DisableVelCorrect()
	{
		velCorrect = false;
	}
	inline bool getVelCorrect()
	{
		bool t = velCorrect;
		velCorrect = true;
		return t;
	}
	virtual entityType GetEntityType() const = 0;

	//Remote
	virtual void SetUpdateTime(float secon){}
	virtual float GetUpdateTime() const { return 0.0f; }
	//Local
	virtual int GetDamage(){ return 0; }
	virtual bool HasDamage() const{ return false; }
	inline unsigned int GetID() const
	{
		return id;
	}
	virtual float GetSpeedModifier() const
	{
		return 1.0f;
	}
	virtual bool AttacksPlanets() const
	{
		return true;
	}
	virtual bool AttacksEntities() const
	{
		return true;
	}

	// 0.0f -> no explosion on death
	virtual float GetExplosionRadius() const
	{
		return 0.0f;
	}
	virtual int GetExplosionDamage() const
	{
		return MAX_HP;
	}
protected:

	PointF vel;

	//PointF target;
	//targetType tarTy = tgPlanet;
	states curState = sLazy;

	bool mood; //current mood of the entity, determines if it will evade to the left or right
	bool bTarReach = false;

	float Tmr = 0.0f;


	bool velCorrect = true;
	const unsigned int id;
};
