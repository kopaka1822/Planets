#pragma once
#include "UIObject.h"

class UIStar : public UIObject
{
public:
	enum type
	{
		EMPTY,
		FILLED,
		NEW // Special effects
	};
	UIStar()
	{}
	virtual ~UIStar()
	{}
	void SetType(type t)
	{
		ty = t;
	}
	virtual void draw(Drawing& draw)
	{
		switch (ty)
		{
		case EMPTY:
			draw.DrawSprite(getRect(), Database::GetTexture(Database::GameTex::StarEmpty));
			break;
		case FILLED:
			draw.DrawSprite(getRect(), Database::GetTexture(Database::GameTex::StartFilled));
			break;
		case NEW:
			draw.DrawSprite(getRect(), Database::GetTexture(Database::GameTex::StartFilled));
			DrawParticles(draw);
			break;
		}
	}
	void Update(float dt)
	{
		if (ty == NEW)
		{
			// rotate particles
			curAngle += 3.0f * dt;
		}
	}
private:
	void DrawParticles(Drawing& draw)
	{
		const PointF mid = getRect().Midpoint();

		PointF vec = PointF(m_dim.x / 2.0f, 0.0f);
		
		vec = vec.rot(curAngle);

		static const int numParticles = 10;

		float step = 2.0f * float(M_PI) / float(numParticles);

		for (int i = 0; i < numParticles; i++)
		{
			draw.DrawParticle(mid + vec, Color::Yellow(), 50.0f);

			vec = vec.rot(step);
		}
	}
private:
	type ty;
	float curAngle = 0.0f;
};