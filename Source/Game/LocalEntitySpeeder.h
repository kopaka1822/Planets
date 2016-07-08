#pragma once
#include "LocalEntity.h"

class LocalEntitySpeeder : public LocalEntity
{
public:
	LocalEntitySpeeder(const PointF& p, byte team, MapObject::targetType ttype, const PointF& tar, int group, bool selec, unsigned int id, const PointF& drawpos)
		:
		LocalEntity(p, team, ttype, tar, group, selec, id, drawpos)
	{}
	virtual entityType GetEntityType() const override final
	{
		return etSpeeder;
	}
	virtual float GetSpeedModifier() const override
	{
		return 2.0f;
	}
};