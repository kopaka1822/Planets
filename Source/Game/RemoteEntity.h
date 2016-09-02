#pragma once
#include "MapEntity.h"

class RemoteEntity : public MapEntity
{
public:
	RemoteEntity(const PointF& p, TeamID team, MapObject::TargetType ttype, const PointF& tar, GroupID group, bool selec, GameID id, EntityType entty, const PointF& drawPos)
		:
		MapEntity(p, team, ttype, tar, group, selec,id),
		bDrawSel(selec),
		oldPos(drawPos),
		etType(entty),
		speedModi((entty == MapObject::etSpeeder)? 2.0f : 1.0f)
	{}
	virtual ~RemoteEntity(){}
	virtual void setUpdateTime(float secon) override
	{
		updTime = secon;
	}
	virtual float getUpdateTime() const override
	{
		return updTime;
	}
	//draw select}
	virtual void drawSelect() override
	{
		bDrawSel = true;
	}
	virtual void drawDeselect() override
	{
		bDrawSel = false;
	}
	virtual bool drawSelected() const override
	{
		return bDrawSel;
	}
	virtual void forceSelect() override
	{
		drawSelect();
		MapObject::forceSelect();
	}
	virtual void deselect() override
	{
		drawDeselect();
		MapObject::deselect();
	}
	virtual void updatePosition(float dt) override
	{
		if (m_curState == MapEntity::States::sTarget)
			MapEntity::updatePosition(dt);
	}
	virtual void calcDrawPos() override
	{
		oldPos = (oldPos * 0.90f + getPos() * 0.10f);
	}
	virtual const PointF & getDrawPos() override
	{
		return oldPos;
	}
	virtual EntityType getEntityType() const override
	{
		return etType;
	}
	virtual float getSpeedModifier() const override
	{
		return speedModi;
	}
private:
	float updTime = 0.0f;
	bool bDrawSel;
	PointF oldPos;
	const EntityType etType;
	const float speedModi;
};