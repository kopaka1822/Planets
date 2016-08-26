#pragma once
#include "LocalEntity.h"

class LocalEntitySaboteur : public LocalEntity
{
public:
	LocalEntitySaboteur(const PointF& p, byte team, MapObject::TargetType ttype, const PointF& tar, int group, bool selec, unsigned int id, const PointF& drawpos)
		:
		LocalEntity(p, team, ttype, tar, group, selec, id, drawpos)
	{
		setHP(2);
	}
	virtual EntityType getEntityType() const override final
	{
		return etSaboteur;
	}
	virtual bool attacksEntities() const override
	{
		return false;
	}
};