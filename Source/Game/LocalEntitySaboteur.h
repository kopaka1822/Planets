#pragma once
#include "LocalEntity.h"

class LocalEntitySaboteur : public LocalEntity
{
public:
	LocalEntitySaboteur(const PointF& p, byte team, MapObject::targetType ttype, const PointF& tar, int group, bool selec, unsigned int id, const PointF& drawpos)
		:
		LocalEntity(p, team, ttype, tar, group, selec, id, drawpos)
	{
		SetHP(2);
	}
	virtual entityType GetEntityType() const override final
	{
		return etSaboteur;
	}
	virtual bool AttacksEntities() const override
	{
		return false;
	}
};