#pragma once
#include "../Utility/Rect.h"
#include <assert.h>

using byte = unsigned char;

class MapObject
{
public:
	enum targetType
	{
		tgPlanet,
		tgPoint,
		tgInvalid,
		tgPlanetDefend
	};
	enum entityType
	{
		etNormal,
		etBomber,
		etSpeeder,
		etSaboteur,
		etNone
	};
protected:
	static float GetEntSpawnFactor(entityType et)
	{
		static const float fac[] = { 1.0f, 3.0f, 1.5f, 12.0f };
		assert(et < 4);
		return fac[et];
	}
public:
	MapObject(PointF position, byte Team, int startHP, targetType ttype, const PointF& tar, int group)
		:
		tarTy(ttype), target(tar),
		pos(position),
		team(Team),
		HP(startHP),
		group(group)
	{}
    virtual ~MapObject(){}
	//interface functions
	//Target
	virtual void invalidateTarget()
	{
		tarTy = tgInvalid;
	}
	const PointF& GetTarget() const
	{
		return target;
	}
	virtual void setTarget(const PointF& tg, targetType tt)
	{
		target = tg;
		tarTy = tt;
	}
	targetType GetTargetType() const
	{
		return tarTy;
	}
	bool hasTarget() const
	{
		return tarTy != tgInvalid;
	}

	//Select

	virtual void deselect()
	{
		isSelected = false;
	}
	bool selected() const
	{
		return isSelected;
	}
	virtual void forceSelect()
	{
		isSelected = true;
	}
	inline void groupSelect(int selGroup)
	{
		if (group == selGroup)
		{
			forceSelect();
		}
		else
		{
			deselect();
		}
	}
	virtual bool drawSelected() const
	{
		return selected();
	}
	virtual void drawSelect()
	{}
	virtual void drawDeselect()
	{}

	//group
	inline void groupAssign(int newGroup)
	{
		group = newGroup;
	}
	inline void groupDestroy(int destGroup)
	{
		if (group == destGroup)
		{
			group = -1;
		}
	}
	inline bool hasGroup() const
	{
		return group != -1;
	}
	inline int GetGroup() const
	{
		return group;
	}

 	//Position + Team

	inline const PointF& GetPos() const
	{
		return pos;
	}

	inline const byte GetTeam() const
	{
		return team;
	}
	inline void SetPosition(const PointF& p)
	{
		pos = p;
	}

	//health
	inline bool TakeDamage(int amount) //returns true if died
	{
		HP -= amount;
		return (HP <= 0);
	}
	inline bool isDead() const
	{
		return (HP <= 0);
	}
	inline int GetHP() const
	{
		return HP;
	}
	inline void SetHP(int n)
	{
		HP = n;
	}

	//collision
	//virtual bool isColliding(const PointF& p) const = 0;
	//virtual bool isNearby(const PointF& p) const = 0;
protected:
	targetType tarTy;
	PointF target;

	PointF pos;
	byte team;
	int HP;
	//user
	bool isSelected = false;
	int group;
};
