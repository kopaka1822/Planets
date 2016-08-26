#pragma once
#include "MapEntity.h"

class LocalEntity : public MapEntity
{
public:
	LocalEntity(const PointF& p, byte team, MapObject::TargetType ttype, const PointF& tar, int group, bool selec, unsigned int id, const PointF& drawpos)
		:
		MapEntity(p,team,ttype,tar,group,selec,id),
		m_oldPos(drawpos)
	{}
	virtual ~LocalEntity(){}

	//Check Has Damage!!!!
	virtual int getDamage() override
	{
		int d = m_charges * DAMAGE;
		m_charges = 0;
		return d;
	}
	virtual bool hasDamage() const override
	{
		return (m_charges != 0);
	}
	virtual void updatePosition(const float dt) override
	{
		m_DmgTmr -= dt;

		if (!m_charges)
		{
			while (m_DmgTmr < 0.0f)
			{
				m_DmgTmr += static_cast<float>(DAMAGE_INTV);
				m_charges = std::min(m_charges + 1, MAX_CHARGES);
			}
		}


		MapEntity::updatePosition(dt);
	}
	virtual void calcDrawPos() override
	{
		m_oldPos = (m_oldPos * 0.70f + getPos() * 0.30f);
	}
	virtual const PointF & getDrawPos() override
	{
		return m_oldPos;
	}

	virtual EntityType getEntityType() const override
	{
		return etNormal;
	}
private:
	float m_DmgTmr = 0.0f;
	int m_charges = 0;
	PointF m_oldPos;
};
