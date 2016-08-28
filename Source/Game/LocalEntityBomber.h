#pragma once
#include "LocalEntity.h"

class LocalEntityBomber : public LocalEntity
{
public:
	LocalEntityBomber(const PointF& p, TeamID team, MapObject::TargetType ttype, const PointF& tar, GroupID group, bool selec, GameID id, const PointF& drawpos)
		:
		LocalEntity(p,team,ttype,tar,group,selec,id, drawpos)
	{
		setHP(1);
	}
	virtual EntityType getEntityType() const override final
	{
		return etBomber;
	}
	virtual bool attacksPlanets() const
	{
		return false;
	}
	virtual bool attacksEntities() const
	{
		return false;
	}
	virtual bool hasDamage() const override
	{
		return false;
	}
	virtual float getExplosionRadius() const
	{
		return 50.0f;
	}
};