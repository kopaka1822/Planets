#pragma once
#include "MapEntity.h"

class RemoteEntity : public MapEntity
{
public:
	RemoteEntity(const PointF& p, byte team, MapObject::targetType ttype, const PointF& tar, int group, bool selec,unsigned int id, entityType entty, const PointF& drawPos)
		:
		MapEntity(p, team, ttype, tar, group, selec,id),
		bDrawSel(selec),
		oldPos(drawPos),
		etType(entty),
		speedModi((entty == MapObject::etSpeeder)? 2.0f : 1.0f)
	{}
	virtual ~RemoteEntity(){}
	virtual void SetUpdateTime(float secon) override
	{
		updTime = secon;
	}
	virtual float GetUpdateTime() const override
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
	virtual void UpdatePosition(const float dt) override
	{
		if (curState == MapEntity::states::sTarget)
			MapEntity::UpdatePosition(dt);
	}
	virtual void CalcDrawPos() override
	{
		oldPos = (oldPos * 0.90f + GetPos() * 0.10f);
	}
	virtual const PointF GetDrawPos() override
	{
		return oldPos;
	}
	virtual entityType GetEntityType() const override
	{
		return etType;
	}
	virtual float GetSpeedModifier() const override
	{
		return speedModi;
	}
private:
	float updTime = 0.0f;
	bool bDrawSel;
	PointF oldPos;
	const entityType etType;
	const float speedModi;
};