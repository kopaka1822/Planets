#pragma once
#include "UIObject.h"
#include <list>
#include "../Utility/LockGuard.h"

class UIUnitFall : public UIObject
{
	struct Entity
	{
		PointF pos;
		PointF vel;
	};
	struct DieEnt
	{
		PointF pos;
		PointF vel;
		float time;
	};
public:
	UIUnitFall(Color c)
		:
		col(c)
	{}
	virtual ~UIUnitFall()
	{}
	virtual void Draw(Drawing& draw) override
	{
		LockGuard g(muEnts);

		// draw standart
		for (const auto& e : ents)
			draw.DrawParticle(e.pos, col, 10.0f);

		for (const auto& e : dieEnts)
		{
			float t = std::max(1.0f - e.time / dieTime, 0.0f);
			Color c = col.Brightness(t);
			draw.DrawParticle(e.pos, c, 5.0f);
		}

		g.Unlock();
	}
	void Update(float dt)
	{
		//PointF vel = PointF(0.0f, dt * float(MapEntity::SPEED));

		PointF gravity = PointF(0.0f, dt * 0.4f);

		LockGuard g(muEnts);
		
		// update position
		for (auto& e : ents)
		{
			e.vel += gravity;
			e.pos += e.vel;
		}

		// update dying positions
		for (auto& e : dieEnts)
		{
			e.pos += (e.vel * dt * float(MapEntity::SPEED));
			e.time += dt;
		}

		// maybe spawn unit?
		lastSpawn += dt;
		if (lastSpawn > spawnTime)
		{
			lastSpawn -= spawnTime;

			// Spawn unit
			float perc = float(rand() % 1000) / 1000.0f;
			SpawnUnit(pos + PointF(dim.x * perc, 0.0f));
		}

		// kill units?
		while (ents.size())
		{
			if (ents.front().pos.y > pos.y + dim.y)
			{
				// die
				PointF diepos = ents.front().pos;
				ents.pop_front();
				AddDying(diepos);
			}
			else break;
		}

		while (dieEnts.size())
		{
			if (dieEnts.front().time > dieTime)
			{
				dieEnts.pop_front();
			}
			else break;
		}

		g.Unlock();
	}
	void SetColor(Color c)
	{
		col = c;
	}
private:
	// must be locked
	void SpawnUnit(PointF pos)
	{
		Entity e;
		e.pos = pos;
		e.vel = PointF(0.0f, 0.0f);
		ents.push_back(e);
	}
	void AddDying(PointF pos)
	{
		int numParts = int(Settings::GetDetailLevel() * 40.0f);
		if (numParts == 0)
			return;

		numParts = (rand() % numParts) + 1;

		DieEnt p;
		p.pos = pos;
		p.time = 0.0f;

		for (int i = 0; i < numParts; i++)
		{
			p.vel = GetRandVec();
			dieEnts.push_back(p);
		}
	}
	PointF GetRandVec()
	{
		PointF velo;
		velo.x = float((rand() % 500) - 250);
		velo.y = float((rand() % 500) - 250);
		if (velo.x == 0.0f && velo.y == 0.0f)
			velo.y = 1.0f;

		velo = velo.normalize();
		return velo;
	}
private:
	Mutex muEnts;
	std::list< Entity > ents;
	std::list< DieEnt > dieEnts;
	const float spawnTime = 0.25f;
	float lastSpawn = spawnTime;
	const float dieTime = 2.0f;
	Color col;
};