#pragma once
#include "LocalEntity.h"

class LocalEntityBomber : public LocalEntity
{
public:
	LocalEntityBomber(const PointF& p, byte team, MapObject::targetType ttype, const PointF& tar, int group, bool selec, unsigned int id, const PointF& drawpos)
		:
		LocalEntity(p,team,ttype,tar,group,selec,id, drawpos)
	{
		SetHP(1);
	}
	virtual entityType GetEntityType() const override final
	{
		return etBomber;
	}
	virtual bool AttacksPlanets() const
	{
		return false;
	}
	virtual bool AttacksEntities() const
	{
		return false;
	}
	virtual bool HasDamage() const override
	{
		return false;
	}
	virtual float GetExplosionRadius() const
	{
		return 50.0f;
	}
};