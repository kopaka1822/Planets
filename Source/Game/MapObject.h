#pragma once
#include "../Utility/Rect.h"
#include <assert.h>

using byte = unsigned char;

class MapObject
{
public:
	enum TargetType
	{
		tgPlanet,
		tgPoint,
		tgInvalid,
		tgPlanetDefend
	};
	enum EntityType
	{
		etNormal,
		etBomber,
		etSpeeder,
		etSaboteur,
		etNone
	};
protected:
	static float getEntSpawnFactor(EntityType et)
	{
		static const float fac[] = { 1.0f, 3.0f, 1.5f, 12.0f };
		assert(et < 4);
		return fac[et];
	}
public:
	MapObject(PointF position, byte Team, int startHP, TargetType ttype, const PointF& tar, int group)
		:
		m_tarTy(ttype), m_target(tar),
		m_pos(position),
		m_team(Team),
		m_hp(startHP),
		m_group(group)
	{}
    virtual ~MapObject(){}
	//interface functions
	//Target
	virtual void invalidateTarget()
	{
		m_tarTy = tgInvalid;
	}
	const PointF& getTarget() const
	{
		return m_target;
	}
	virtual void setTarget(const PointF& tg, TargetType tt)
	{
		m_target = tg;
		m_tarTy = tt;
	}
	TargetType getTargetType() const
	{
		return m_tarTy;
	}
	bool hasTarget() const
	{
		return m_tarTy != tgInvalid;
	}

	//Select

	virtual void deselect()
	{
		m_isSelected = false;
	}
	bool selected() const
	{
		return m_isSelected;
	}
	virtual void forceSelect()
	{
		m_isSelected = true;
	}
	inline void groupSelect(int selGroup)
	{
		if (m_group == selGroup)
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

	//m_group
	inline void groupAssign(int newGroup)
	{
		m_group = newGroup;
	}
	inline void groupDestroy(int destGroup)
	{
		if (m_group == destGroup)
		{
			m_group = -1;
		}
	}
	inline bool hasGroup() const
	{
		return m_group != -1;
	}
	inline int getGroup() const
	{
		return m_group;
	}

 	//Position + Team

	inline const PointF& getPos() const
	{
		return m_pos;
	}

	inline byte getTeam() const
	{
		return m_team;
	}
	inline void setPosition(const PointF& p)
	{
		m_pos = p;
	}

	//health
	inline bool takeDamage(int amount) //returns true if died
	{
		m_hp -= amount;
		return (m_hp <= 0);
	}
	inline bool isDead() const
	{
		return (m_hp <= 0);
	}
	inline int getHP() const
	{
		return m_hp;
	}
	inline void setHP(int n)
	{
		m_hp = n;
	}

	//collision
	//virtual bool isColliding(const PointF& p) const = 0;
	//virtual bool isNearby(const PointF& p) const = 0;
protected:
	TargetType m_tarTy;
	PointF m_target;

	PointF m_pos;
	byte m_team;
	int m_hp;
	//user
	bool m_isSelected = false;
	int m_group;
};
