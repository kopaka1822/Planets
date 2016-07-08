#pragma once
#include "UIObject.h"
#include <list>
#include "../Utility/LockGuard.h"

class UIPlanet : public UIObject
{
	struct Entity
	{
		Entity(PointF velo)
			:
			vel(velo){}
		PointF pos;
		const PointF vel;
		float time;
	};
public:
	UIPlanet(Color c, float radius, unsigned long long& score)
		:
		c(c),
		radius(radius),
		score(score)
	{

	}
	virtual ~UIPlanet()
	{

	}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawPlanet(pos, c, radius);
	}
	virtual void DrawEntities(Drawing& draw) const
	{
		LockGuard guard(muDraw);

		auto len = ents.size();
		if (len)
		{
			std::unique_ptr< PointF[] > points = std::unique_ptr< PointF[] >(new PointF[len]);
			PointF* cur = points.get();

			for (const auto& e : ents)
				*cur++ = e.pos;

			guard.Unlock();

			draw.DrawParticleArray(points.get(), len, c, 30.0f);
		}
		else
			guard.Unlock();
	}
	void Update(float dt)
	{
		LockGuard guard(muDraw);

		for (auto& e : ents)
		{
			e.pos = e.pos +  (e.vel * 10.0f * dt);
			e.time += dt;
		}
		guard.Unlock();
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		//kill entity dhuhu
		if (k != Input::Left)
			return;

		auto tar = ents.end();

		for (auto e = ents.begin(), end = ents.end(); e != end; ++e)
		{
			if ((e->pos - pos).lengthSq() < 300.0f)
			{
				tar = e;
				break;
			}
		}

		if (tar != ents.end())
		{
			muDraw.Lock();
			ents.erase(tar);
			muDraw.Unlock();
			Sound::Effect(Sound::S_DIE);
			score += 10;
			Settings::UnlockAchievement(Settings::A_KillMenu);
		}

	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		if ((this->pos - pos).lengthSq() < radius * radius)
		{
			//Spawn entity

			//GetRandom Vector
			PointF velo;
			velo.x = float((rand() % 500) - 250);
			velo.y = float((rand() % 500) - 250);
			if (velo.x == 0.0f && velo.y == 0.0f)
				velo.x = 1.0f;
			velo = velo.normalize();

			Entity e(velo);

			e.pos = (e.vel * radius) + this->pos;
			e.time = 0.0f;

			//list full?
			muDraw.Lock();
			if (ents.size() == maxPart - 1)
				ents.pop_front();

			while (ents.size())
			{
				if (ents.front().time > 120.0f)
				{
					ents.pop_front();
				}
				else break;
			}

			ents.push_back(e);
			Sound::Effect(Sound::S_SPAWN);
			score++;
			muDraw.Unlock();
		}
	}
	void SetColor(Color c)
	{
		this->c = c;
	}
private:
	const float radius;
	Color c;
	static const unsigned int maxPart = 1000;
	Mutex muDraw;

	std::list< Entity > ents;

	unsigned long long& score;
};
