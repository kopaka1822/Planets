#pragma once
#include "../Utility/Rect.h"
#include "MapLoader.h"
#include "MapObject.h"

using byte = unsigned char;

class MapPlanet : public MapObject
{
public:
	MapPlanet(const MapLoader::MapPlanet& plan, unsigned int ID)
		:
		MapObject({ plan.x, plan.y }, plan.team, plan.HP,MapObject::tgPlanetDefend,PointF(ID,0.0f),-1),
        sUnit(plan.sUnit),
		radius(plan.radius),
		colRect(RectF(plan.x - plan.radius, plan.y - plan.radius, plan.x + plan.radius, plan.y + plan.radius)),
		r2(radius * radius),
		nearR(radius + PLAN_NEAR),
		nearR2( nearR * nearR),
		defR(radius + float(PLAN_DEFENSE)),
		defR2(defR * defR),
		MaxHP(plan.HP),
        ID(ID),
        curEntType(etNormal)
	{
	}
	virtual ~MapPlanet(){}

	float GetRadius() const
	{
		return radius;
	}
	float GetDefenseRadius() const
	{
		return defR;
	}
	virtual bool Update(const float dt) = 0;
	inline bool isColliding(const PointF& p) const
	{
		/*if (fabs(p.x - pos.x) <= radius)
			if (fabs(p.y - pos.y) <= radius)
				if ((p - pos).lengthSq() <= r2)
					return true;*/
		if (colRect.PointInside(p))
		{
			if ((p - pos).lengthSq() <= r2)
			{
				return true;
			}
		}
		return false;
	}
	inline bool isNearby(const PointF& p) const
	{
		if (fabs(p.x - pos.x) <= nearR)
			if (fabs(p.y - pos.y) <= nearR)
				if ((p - pos).lengthSq() <= nearR2)
					return true;

		return false;
	}
	//inside defense radius
	inline bool isInDefend(const PointF& p) const
	{
		if (fabs(p.x - pos.x) <= defR)
			if (fabs(p.y - pos.y) <= defR)
				if ((p - pos).lengthSq() <= defR2)
					return true;

		return false;
	}
	bool TakeDamage(int amount, byte team) //returns true if captured
	{
		//if (this->team != team)
		//{
			if (this->team != 0) // someone owns this planet
			{
				HP -= amount;
				if (HP <= 0)
				{
					TakeOver(team);
					return true;
				}
			}
			else
			{//noone owns this planet
				if (HP == MaxHP)
				{
					subteam = team;
				}

				if (subteam == team)
				{
					HP -= amount;
					if (HP <= 0)
					{
						TakeOver(team);
						return true;
					}
				}
				else  //subteam != team
				{
					HP += amount;
					if (HP >= MaxHP)
					{
						HP = MaxHP;
						subteam = 0;
					}
				}
			}
		//}
		return false;
	}
	int GetMaxHP() const
	{
		return MaxHP;
	}
	float GetPercentage() const
	{
		return float(HP) / float(MaxHP);
	}
	virtual float GetDrawPercentage()
	{
		return GetDrawPercentage();
	}
	virtual void CalcDrawPercentage()
	{}
	short GetID() const //-1 = invalid
	{
		return ID;
	}
	virtual void TakeOver(byte team)
	{
		HP = MaxHP;
		if (this->team == 0)
		{
			this->team = team;
		}
		else
		{
			this->team = 0;
		}
		subteam = 0;
		//reset some things..
		MapObject::deselect();
		MapObject::group = -1;
		MapObject::setTarget({ ID, 0.0f }, tgPlanetDefend);
		curEntType = etNormal;
	}
	virtual void Sabotage(byte tea)
	{
		if (this->team == tea)
			return;

		if (this->team != 0)
		{
			TakeOver(0);
		}
		else
		{
			TakeOver(tea);
		}
	}
	void SetEntityType(entityType t)
	{
		curEntType = t;
	}
	entityType GetEntityType() const
	{
		return curEntType;
	}
	//Local
	virtual float GetsUnit() const
	{
		return 0.0f;
	}
	byte GetSubteam() const
	{
		return subteam;
	}
	int GetSubHP(byte ownTeam) const
	{
		if (subteam)
		{
			if (subteam == ownTeam)
			{
				return HP;
			}
			else
			{
				return 2 * MaxHP - HP;
			}
		}
		else
		{
			return 2 * HP;
		}
	}
	void SetSubAndHP(byte sub, int HP)
	{
		SetHP(HP);
		subteam = sub;
	}

	virtual float GetSpawnTimePercent() = 0;
	virtual void ResetSpawnTime(){}
protected:
	float GetSpawnFactor() const
	{
		//	              1
		//  f(x) =  --------------
		//			0.9 * x² + 0,1
		float perc = GetPercentage();
		float y = 0.9f * perc * perc + 0.1f;
		return 1.0f / y * GetEntSpawnFactor(GetEntityType());
	}
protected:
	const float sUnit;
	const float radius;
	const RectF colRect;
	const float r2;
	const float nearR;
	const float nearR2;

	const float defR;
	const float defR2;

	const int MaxHP;

	const short ID;

	byte subteam = 0;

	entityType curEntType;
public:
	static const int PLAN_DEFENSE = 20;
	static const int PLAN_NEAR = 15;
};
