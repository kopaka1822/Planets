#pragma once
#include "LocalEntity.h"

class LocalEntitySpeeder : public LocalEntity
{
public:
	LocalEntitySpeeder(const PointF& p, TeamID team, MapObject::TargetType ttype, const PointF& tar, GroupID group, bool selec, GameID id, const PointF& drawpos)
		:
		LocalEntity(p, team, ttype, tar, group, selec, id, drawpos)
	{}
	virtual EntityType getEntityType() const override final
	{
		return etSpeeder;
	}
	virtual float getSpeedModifier() const override
	{
		return 2.0f;
	}
};