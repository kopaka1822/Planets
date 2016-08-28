#pragma once
#include "LocalEntity.h"

class LocalEntitySaboteur : public LocalEntity
{
public:
	LocalEntitySaboteur(const PointF& p, TeamID team, MapObject::TargetType ttype, const PointF& tar, GroupID group, bool selec, GameID id, const PointF& drawpos)
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