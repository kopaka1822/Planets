#pragma once
#include "MapEntity.h"

class LocalEntity : public MapEntity
{
public:
	LocalEntity(const PointF& p, byte team, MapObject::targetType ttype, const PointF& tar, int group, bool selec, unsigned int id, const PointF& drawpos)
		:
		MapEntity(p,team,ttype,tar,group,selec,id),
		oldPos(drawpos)
	{}
	virtual ~LocalEntity(){}

	//Check Has Damage!!!!
	virtual int GetDamage() override
	{
		int d = charges * DAMAGE;
		charges = 0;
		return d;
	}
	virtual bool HasDamage() const
	{
		return (charges != 0);
	}
	virtual void UpdatePosition(const float dt) override
	{
		DmgTmr -= dt;

		if (!charges)
		{
			while (DmgTmr < 0.0f)
			{
				DmgTmr += (float)DAMAGE_INTV;
				charges = std::min(charges + 1, MAX_CHARGES);
			}
		}


		MapEntity::UpdatePosition(dt);
	}
	virtual void CalcDrawPos() override
	{
		oldPos = (oldPos * 0.70f + GetPos() * 0.30f);
	}
	virtual const PointF GetDrawPos() override
	{
		return oldPos;
	}

	virtual entityType GetEntityType() const override
	{
		return etNormal;
	}
private:
	float DmgTmr = 0.0f;
	int charges = 0;
	PointF oldPos;
};
