#pragma once
#include "../Game/Map.h"
#include "Drawing.h"
#include <deque>
#include "UIGridEntity.h"
#include "../Utility/LockGuard.h"
#include "../Utility/Tools.h"

class MapDrawer : public Map::EventReciever
{
	struct explosionParticle
	{
		PointF pos;
		PointF vel;
		float time;
		Color col;
	};
	const float dieTime = 2.0f;
	const float capTime = 4.0f;
public:
	MapDrawer(Map& map, byte team)
		:
		map(map),
		myTeam(team),
		camPos(0.0f, 0.0f),
		camDes(0.0f, 0.0f),
		scalar(4.0f), // animation from upper left
		scalarDes(4.0f),
		mWidth(map.GetWidth()), mHeight(map.GetHeight()),
		entSelect(team)
	{
		map.addEventReciever(this);
		entSelect.disable();
		planColors.assign(map.nPlans, Color::White());
		for (const auto& p : map.plans)
			planColors[p->getID()] = Color::GetTeamColor(p->getTeam());

		lastPlanSpawn.assign(map.nPlans, 1000.0f);
		//Zoom(1.0f);
	}
	void DisableEntSelect()
	{
		bEntSelectEnable = false;
		entSelect.disable();
	}
	UIGridEntity* GetEntitySpawnSelect()
	{
		return &entSelect;
	}
	void Draw(Drawing& draw, const PointF& mouse)
	{
		// set new scalar and cam
		float smooth = 0.8f;
		float invSmooth = 1.0f - smooth;

		scalar = smooth * scalar + invSmooth * scalarDes;
		camPos = camPos * smooth + camDes * invSmooth;

		DrawDieEnts(draw);
		DrawCapPlans(draw);
		//Lock ents
		if (Settings::TargetLinesEnabled())
			DrawTargetLines(draw);

		
		
		if (entSelect.isEnabled())
		{
			DrawLinesToEntSelect(draw);
			/*draw.DrawLine(, fromModel(p.GetPos()), Color::White().Transparent(0.9f), 3.0f);
			draw.DrawPlanet(fromModel(p.GetPos()), Color::GetTeamColor(p.GetTeam()), fromModel(p.GetRadius()));*/
		}
		
		DrawPlanets(draw);

		DrawEnts(draw);

		if (RectF(pos, pos + dim).PointInside(mouse))
		{
			DrawPlanetBar(draw, toModel(mouse));
		}
		if (bEntSelectEnable)
			DrawPlanetCircle();

		draw.DrawTransBox(RectF(fromModel({ 0.0f, 0.0f }),fromModel({ mWidth, mHeight })), 2.0f, Color::Gray().Brightness(0.2f));


		if (entSelect.isEnabled())
		{
			entSelect.draw(draw);
		}

		


	}
	void Update(float dt)
	{
		//Update dying ents
		for (auto& e : dieEnts)
		{
			e.pos = e.pos + (e.vel * dt * float(MapEntity::SPEED));
			e.time += dt;
		}
		{
			muDieEnts.Lock();
			while (!dieEnts.empty())
			{
				if (dieEnts.front().time <= dieTime)
					break;

				dieEnts.pop_front();
			}
			muDieEnts.Unlock();
		}

		//Update Cap Plans
		for (auto& p : capPlans)
		{
			p.pos = p.pos + (p.vel * dt * float(MapEntity::SPEED));
			p.time += dt;
		}
		{
			muCapPlan.Lock();
			while (!capPlans.empty())
			{
				if (capPlans.front().time <= capTime)
					break;

				capPlans.pop_front();
			}
			muCapPlan.Unlock();
		}

		for (auto& s : lastPlanSpawn)
			s += dt;
	}
	void SetOrigin(const PointF& p)
	{
		pos = p;
	}
	void SetMetrics(const PointF& d)
	{
		dim = d;

		if (dim.x * mHeight < dim.y * mWidth) // height cam
		{
			CamArea = RectF(PointF(0, -(mWidth * dim.y / dim.x - mHeight) / 2), PointF(mWidth, mHeight / 2 + mWidth * dim.y / (2 * dim.x)));
		}
		else if (dim.x * mHeight >= dim.y * mWidth) // width cam
		{
			CamArea = RectF(PointF(-(mHeight * dim.x / dim.y - mWidth) / 2, 0), PointF(mWidth / 2 + mHeight * dim.x / (2 * dim.y), mHeight));
			//CamArea = RectF(PointF(0, 0), PointF(mWidth, mHeight));
		}

		if (bFirstOrigin)
		{
			// set camera to watch whole map
			Zoom(0.001f,PointF(0.0f,0.0f));

			bFirstOrigin = false;
		}
	}
	RectF GetRect() const
	{
		return RectF(pos, pos + dim);
	}

	// special events
	void SetPlanetBlur(PlanetID pid)
	{
		auto p = map.getPlanP(pid);
		if (p)
		{
			Drawing::GetDraw().SetBlur(fromModel(p->getPos()));
		}
	}
private:
	void DrawLinesToEntSelect(Drawing& draw)
	{
		const PointF center = entSelect.getRect().GetMidpoint();
		for (const auto& p : map.plans)
		{
			if (p->getTeam() == myTeam && p->selected())
				draw.DrawLine(center, fromModel(p->getPos()), Color::White(), 3.0f);
		}
	}
	void DrawPlanetCircle()
	{
		// selection circle
		PlanetID pid = 0;
		int numSel = 0;

		bool bSameType = true;
		MapObject::EntityType curType = MapObject::EntityType::etNormal;

		for (const auto& p : map.plans)
		{
			if (p->selected() && p->getTeam() == myTeam)
			{
				pid = p->getID();
				if (numSel == 0)
				{
					curType = p->getEntityType();
				}
				else if (curType != p->getEntityType())
				{
					bSameType = false;
				}

				numSel++;
			}
		}

		// entity selector
		if (numSel >= 1)
		{
			const PointF p = map.getPlan(pid).getPos();

			PointF off = PointF(fromModel(map.getPlan(pid).getRadius()), fromModel(map.getPlan(pid).getRadius()));
			RectF colBox = RectF(fromModel(p) - off, fromModel(p) + off);

			// exactly one planet is selected -> draw cricle
			//entSelect.SetCenter(fromModel(map.plans[pid]->GetPos()));
			const float xMin = Framework::DrawStart().x + 10.0f;
			const float xMax = Framework::DrawStart().x + Framework::DrawWidth().x - 10.0f - entSelect.getMetrics().x;

			float x = fromModel(map.getPlan(pid).getPos()).x - entSelect.getMetrics().x / 2.0f;
			x = tool::clamp(x, xMin, xMax);

			//entSelect.setOrigin({ x, Framework::DrawStart().y + 10.0f });
			//entSelect.centerX(Framework::DrawStart().y + Framework::DrawWidth().y - entSelect.getMetrics().y);
			entSelect.setOrigin(Framework::DrawStart() + PointF(10, 10));

			if (bSameType)
			{
				entSelect.SetActiveType(curType);
			}
			else
			{
				entSelect.SetActiveType(MapObject::EntityType::etNone);
			}
			//entSelect.SetActiveType(map.getPlan(pid).GetEntityType(), pid);
			entSelect.enable();

		}
		else
		{
			entSelect.disable();
		}
	}
	void DrawPlanets(Drawing& draw)
	{
		for (const auto& p : map.plans)
		{
			p->calcDrawPercentage();
			// update color
			planColors[p->getID()] = planColors[p->getID()].mix(Color::GetTeamColor(p->getTeam()), 0.92f);


			//draw.DrawPlanet(fromModel(p->GetPos()), planColors[p->GetID()], fromModel(p->GetRadius()));

			if (p->drawSelected() && p->getTeam() == myTeam)
			{
				//draw.DrawCircle(fromModel(p->GetPos()),fromModel( p->GetRadius() - 6.0f), Color::White(),2.0f);
				draw.DrawCircle(fromModel(p->getPos()), fromModel(p->getDefenseRadius()), Color::Blue(), 2.0f);
			}

			float blend = float(p->getHP()) / float(p->getMaxHP());
			Color c = Color::GetTeamColor(p->getTeam()).mix(Color::GetTeamColor(p->getSubteam()), blend);

			float lastSpawn = lastPlanSpawn[p->getID()];
			if (p->getTeam() == 0)
				lastSpawn = 1000.0f;

			draw.DrawPlanet(fromModel(p->getPos()), planColors[p->getID()], c, fromModel(p->getRadius()),lastSpawn / p->getsUnit());
			//draw.DrawSpriteColored(RectF::FromPoint(fromModel(p->GetPos()), fromModel(p->GetRadius())), Database::GetTexture(Database::PlanetDes));
		}
	}
	void DrawEnts(Drawing& draw)
	{
		// draw each m_team

		struct PartArr
		{
			PointF* arr; // array
			PointF* cur;
			size_t count;
		};

		PartArr norm;
		PartArr normwh; // white

		PartArr bomb;
		PartArr bombwh;

		PartArr speed;
		PartArr speedwh;

		PartArr sabo;
		PartArr saboWh;

		for (size_t i = 0; i < map.nPlayers; i++)
		{
			tool::zeroStruct(&norm);

			if (i + 1 == myTeam)
			{
				tool::zeroStruct(&normwh);
				tool::zeroStruct(&bomb);
				tool::zeroStruct(&bombwh);
				tool::zeroStruct(&speed);
				tool::zeroStruct(&speedwh);
				tool::zeroStruct(&sabo);
				tool::zeroStruct(&saboWh);
			}

			LockGuard guard(map.muEnts);

			size_t len = map.ents[i].length();
			if (len == 0)
				continue;

			// allocate arrays
			norm.arr = new PointF[len];
			norm.cur = norm.arr;

			if (i + 1 == myTeam)
			{
				normwh.arr = new PointF[len];
				normwh.cur = normwh.arr;

				bomb.arr = new PointF[len];
				bomb.cur = bomb.arr;
				bombwh.arr = new PointF[len];
				bombwh.cur = bombwh.arr;

				speed.arr = new PointF[len];
				speed.cur = speed.arr;
				speedwh.arr = new PointF[len];
				speedwh.cur = speedwh.arr;

				sabo.arr = new PointF[len];
				sabo.cur = sabo.arr;
				saboWh.arr = new PointF[len];
				saboWh.cur = saboWh.arr;
			}

			// fill arrays

			if (i + 1 == myTeam)
			{
				// special units
				for (auto& e : map.ents[i])
				{
					e.calcDrawPos();

					if (e.selected())
					{
						// white
						switch (e.getEntityType())
						{
						case MapObject::etNormal:
							*normwh.cur = e.getDrawPos();
							normwh.cur++;
							normwh.count++;
							break;
						case MapObject::etBomber:
							*bombwh.cur = e.getDrawPos();
							bombwh.cur++;
							bombwh.count++;
							break;
						case MapObject::etSaboteur:
							*saboWh.cur = e.getDrawPos();
							saboWh.cur++;
							saboWh.count++;
							break;
						case MapObject::etSpeeder:
							*speedwh.cur = e.getDrawPos();
							speedwh.cur++;
							speedwh.count++;
							break;
						}
					}
					else
					{
						// m_team color
						switch (e.getEntityType())
						{
						case MapObject::etNormal:
							*norm.cur = e.getDrawPos();
							norm.cur++;
							norm.count++;
							break;
						case MapObject::etBomber:
							*bomb.cur = e.getDrawPos();
							bomb.cur++;
							bomb.count++;
							break;
						case MapObject::etSaboteur:
							*sabo.cur = e.getDrawPos();
							sabo.cur++;
							sabo.count++;
							break;
						case MapObject::etSpeeder:
							*speed.cur = e.getDrawPos();
							speed.cur++;
							speed.count++;
							break;
						}
					}
				}
			}
			else
			{
				// just normal in one color
				for (auto& e : map.ents[i])
				{
					e.calcDrawPos();
					*norm.cur = e.getDrawPos();
					norm.cur++;
					norm.count++;
				}
			}

			guard.Unlock();

			// draw arrays
			static const float sz = 15.0f;

			if (norm.count)
				draw.DrawParticleArray(norm.arr, norm.count, Color::GetTeamColor(i + 1), sz, Database::GameTex::Particle, camPos, scalar, pos);

			tool::safeDeleteArray(norm.arr);

			if (i + 1 == myTeam)
			{
				// draw other units
				if (bomb.count)
					draw.DrawParticleArray(bomb.arr, bomb.count, Color::GetTeamColor(i + 1), sz, Database::GameTex::ParticleExpl, camPos, scalar, pos);
				tool::safeDeleteArray(bomb.arr);

				if (sabo.count)
					draw.DrawParticleArray(sabo.arr, sabo.count, Color::GetTeamColor(i + 1), sz, Database::GameTex::ParticleSabo, camPos, scalar, pos);
				tool::safeDeleteArray(sabo.arr);

				if (speed.count)
					draw.DrawParticleArray(speed.arr, speed.count, Color::GetTeamColor(i + 1), sz, Database::GameTex::ParticleTank, camPos, scalar, pos);
				tool::safeDeleteArray(speed.arr);

				// draw in white
				if (normwh.count)
					draw.DrawParticleArray(normwh.arr, normwh.count, Color::White(), sz, Database::GameTex::Particle, camPos, scalar, pos);
				tool::safeDeleteArray(normwh.arr);

				if (saboWh.count)
					draw.DrawParticleArray(saboWh.arr, saboWh.count, Color::White(), sz, Database::GameTex::ParticleSabo, camPos, scalar, pos);
				tool::safeDeleteArray(saboWh.arr);

				if (bombwh.count)
					draw.DrawParticleArray(bombwh.arr, bombwh.count, Color::White(), sz, Database::GameTex::ParticleExpl, camPos, scalar, pos);
				tool::safeDeleteArray(bombwh.arr);

				if (speedwh.count)
					draw.DrawParticleArray(speedwh.arr, speedwh.count, Color::White(), sz, Database::GameTex::ParticleTank, camPos, scalar, pos);
				tool::safeDeleteArray(speedwh.arr);
			}
		}
	}
	void DrawPlanetBar(Drawing& draw, const PointI& ms)
	{
		for (const auto& p : map.plans)
		{
			if (p->isNearby(ms))
			{
				DrawBar(draw, p->getDrawPercentage(), fromModel(p->getPos()), fromModel(p->getRadius()),
					Color::GetTeamColor(p->getSubteam()),p->getSpawnTimePercent(), p->getTeam() == myTeam);
			}
		}
	}
	void DrawBar(Drawing& draw, float percent, const PointF& plan, float radius, Color cb, float entPercent, bool drawPerc)
	{
		const float width = 150.0f;
		const float height = 20.0f;
		const float padding = 5.0f;
		const float dist = 20.0f;

		const float perStart = 24.0f;
		const float perHi = 10.0f;
		const float perPad = 1.0f;

		float startX = plan.x - width / 2.0f;
		float startY = plan.y - radius - height - dist;

		startX = std::max(0.0f, startX);
		startX = std::min(Framework::STD_DRAW_X - 1 - width, startX);

		if (startY < 0)
		{
			startY = plan.y + radius + height + dist;
		}

		draw.DrawBox({ startX, startY ,startX + width, startY + height }, 2.0f, cb, Color::Black());
		if (drawPerc)
		{
			draw.DrawBox({ startX, startY + perStart, startX + width, startY + perStart + perHi }, 2.0f, Color::White(), Color::Black());
		}

		float barWidth = (width - 2.0f * padding) * percent;

		Color c = Color::Green();
		if (percent < 0.4f)
			c = Color::Yellow();
		if(percent < 0.1f)
			c = Color::Red();

		if (barWidth > 0)
		{
			draw.DrawRect({ startX + padding, startY + padding, startX + padding + barWidth, startY + height - padding }, c);
		}

		if (drawPerc)
		{
			barWidth = width * entPercent;
			if (barWidth > 0)
			{
				draw.DrawRect({ startX, startY + perStart, startX + barWidth, startY + perStart + perHi }, Color::White());
			}
		}
	}
	void DrawTargetLines(Drawing& draw)
	{
		const Color col = Color(0.3f, 0.3f, 0.3f, 0.5f);
		const float stroke = 1.5f;

		map.muEnts.Lock();
		for (auto& e : map.ents[myTeam - 1])
		{
			if (e.drawSelected())
			{
				if (e.hasTarget())
				{
					switch (e.getTargetType())
					{
					case MapObject::tgPlanet:
					{
						MapPlanet* t = map.getPlanP((PlanetID)e.getTarget().x); //this is necessary!
						if (t)
							draw.DrawLine(fromModel(e.getDrawPos()), fromModel(t->getPos()), col, stroke);
					}
						break;
					case MapObject::tgPlanetDefend:
						break;
					default:
						draw.DrawLine(fromModel(e.getDrawPos()), fromModel(e.getTarget()), col, stroke);
					}
				}
			}
		}
		map.muEnts.Unlock();

		for (const auto& p : map.plans)
		{
			if (p->getTeam() == myTeam)
			{
				if (p->drawSelected())
				{
					if (p->hasTarget())
					{
						switch (p->getTargetType())
						{
						case MapObject::tgPlanet:
						{
							MapPlanet* t = map.getPlanP((PlanetID)p->getTarget().x); //this is necessary!
							if (t)
								draw.DrawLine(fromModel(p->getPos()), fromModel(t->getPos()), col, stroke);
						}
							break;
						case MapObject::tgPlanetDefend:
							break;
						default:
							draw.DrawLine(fromModel(p->getPos()), fromModel(p->getTarget()), col, stroke);
						}
					}
						//if (p->GetTargetType() != MapObject::tgPlanetDefend)
							//draw.DrawLine(fromModel(p->GetPos()), fromModel(p->GetTarget()), Color::Gray(), 1.0f);
				}
			}
		}
	}
	void DrawDieEnts(Drawing& draw)
	{
		muDieEnts.Lock();
		for (const auto& e : dieEnts)
		{
			float t = std::max(1.0f - e.time / dieTime, 0.0f);
			Color c = e.col.Brightness(t);
			draw.DrawParticle(fromModel(e.pos), c, fromModel(5.0f));
		}
		muDieEnts.Unlock();
	}
	void DrawCapPlans(Drawing& draw)
	{
		muCapPlan.Lock();
		for (const auto& p : capPlans)
		{
			float t = std::max(1.0f - p.time / capTime,0.0f);
			Color c = p.col.Brightness(t);
			draw.DrawParticle(fromModel(p.pos), c, fromModel(20.0f));
		}
		muCapPlan.Unlock();
	}

public:
	void MoveCam(float dx, float dy)
	{
		camDes.x += dx;
		camDes.y += dy;

		float minx = 0.0f;
		float miny = 0.0f;

		/*if (fromModelDes(mWidth) < dim.x)
		{
		minx = (dim.x - fromModelDes(mWidth));
		minx = (dim.x / scalar) / 2.0f;
		}
		if (fromModelDes(mHeight) < dim.y)
		{
		miny = (dim.y - fromModelDes(mHeight));
		miny = (dim.y / scalar) / 2.0f;
		}


		camDes.x = clamp(camDes.x, -minx, mWidth - dim.x / scalarDes + minx);
		camDes.y = clamp(camDes.y, -miny, mHeight - dim.y / scalarDes + miny);*/
		CheckForOverlap();
	}

	void SetScalar(float sc)
	{
		scalarDes = sc;
		scalar = sc;
	}
	void Zoom(float s)
	{
		ZoomNoMove(s);
	}
	void Zoom(float s, PointF center)
	{
		PointF pos = toModelDes(center);
		PointF d = (center - PointF(640, 325));
		PointF dNew = (PointF(640, 325) - d);

		SetCamNoBorder(pos);
		ZoomNoBorder(s);
		SetCamNoBorder(toModelDes(dNew));

		CheckForOverlap();
	}

	// p in model coords
	void SetCam(const PointF& p)
	{
		const RectF cRect = GetCamRect();

		camDes = p - PointF(cRect.getWidth() / 2.0f, cRect.getHeight() / 2.0f);
		//MoveCam(0.0f, 0.0f);

		CheckForOverlap();
	}
	// Helper scaling functions
	PointF fromModel(PointF p)
	{
		return (p - camPos) * scalar + pos;
	}
	float fromModel(float s)
	{
		return s * scalar;
	}
	float fromModelDes(float s)
	{
		return s * scalarDes;
	}
	PointF GetCam() const
	{
		return camPos;
	}
	float GetScalar() const
	{
		return scalar;
	}
	PointF toModel(PointF p)
	{
		p = p - pos;
		p = p / scalar;
		p = p + camPos;
		//p = p / scalar;
		return p;
	}
	PointF toModelDes(PointF p)
	{
		p = p - pos;
		p = p / scalarDes;
		p = p + camDes;
		//p = p / scalar;
		return p;
	}
	float toModel(float s)
	{
		return s / scalar;
	}
	float toModelDes(float s)
	{
		return s / scalarDes;
	}
	RectF GetCamRect() const
	{
		return RectF(camDes, camDes + (dim / scalarDes));
	}
	RectF GetCamRectSmooth() const
	{
		return RectF(camPos, camPos + (dim / scalar));
	}
	void SetCamMaxSize()
	{
		float s = (GetCamRect().getWidth() / mWidth);
		Zoom(s);
	}
private:
	void SetCamNoBorder(const PointF& p)
	{
		const RectF cRect = GetCamRect();

		camDes = p - PointF(cRect.getWidth() / 2.0f, cRect.getHeight() / 2.0f);

		float minx = 0.0f;
		float miny = 0.0f;

		if (fromModelDes(mWidth) < dim.x)
		{
			minx = (dim.x - fromModelDes(mWidth));
			//minx = (dim.x / scalar) / 2.0f;
		}
		if (fromModelDes(mHeight) < dim.y)
		{
			miny = (dim.y - fromModelDes(mHeight));
			//miny = (dim.y / scalar) / 2.0f;
		}
	}
	void ZoomNoBorder(float s)
	{
		float sb = scalarDes;
		float ox = toModelDes(dim.x);
		float oy = toModelDes(dim.y);

		ZoomNoMove(s);

		float dx = (ox - toModelDes(dim.x)) / 2.0f;
		float dy = (oy - toModelDes(dim.y)) / 2.0f;

		camDes.x += dx;
		camDes.y += dy;

		float minx = 0.0f;
		float miny = 0.0f;

		if (fromModelDes(mWidth) < dim.x)
		{
			minx = (dim.x - fromModelDes(mWidth));
			//minx = (dim.x / scalar) / 2.0f;
		}
		if (fromModelDes(mHeight) < dim.y)
		{
			miny = (dim.y - fromModelDes(mHeight));
			//miny = (dim.y / scalar) / 2.0f;
		}
	}
	void CheckForOverlap()
	{
		int Counter = 0;

		//check for full size

		if (CamArea.RectFullInside(GetCamRect()))
		{
			return;
		}

		for (int i = 0; i < 4; i++)
		{
			if (CamArea.PointInside(GetCamRect().GetVector()[i]))
				Counter++;
		}
		//Deep magic of my life
		switch (Counter)
		{
		case 1:
			for (int i = 0; i < 4; i++)
			{
				if (CamArea.PointInside(GetCamRect().GetVector()[i]))
				{
					switch (i)
					{
					case 2:
						camDes = CamArea.TopLeft();
						goto end;
					case 3:
						camDes = PointF(CamArea.getWidth() / 2 + mWidth / 2 - GetCamRect().getWidth(), CamArea.y1);
						goto end;
					case 1:
						camDes = PointF(CamArea.x1, CamArea.getHeight() / 2 + mHeight / 2 - GetCamRect().getHeight());
						goto end;
					case 0:
						camDes = CamArea.BottomRight() - GetCamRect().GetMetrics();
						goto end;
					default:
						goto end;
					}
				}
			}
		end:
			break;

		case 2:
			for (int i = 0; i < 4; i++)
			{
				if (CamArea.PointInside(GetCamRect().GetVector()[i]))
				{
					if (CamArea.PointInside(GetCamRect().GetVector()[(i + 1) % 4]))
					{
						switch (i)
						{
						case 2:
							camDes = PointF(GetCamRect().TopLeft().x, CamArea.y1);
							goto ent;
						case 3:
							camDes = PointF(CamArea.getWidth() / 2 + mWidth / 2 - GetCamRect().getWidth(), GetCamRect().TopLeft().y);
							goto ent;
						case 1:
							camDes = PointF(CamArea.x1, GetCamRect().TopLeft().y);
							goto ent;
						case 0:
							camDes = PointF(GetCamRect().x1, CamArea.getHeight() / 2 + mHeight / 2 - GetCamRect().getHeight());
							goto ent;
						}
					}
				}
			}
		ent:
			break;
		case 0:
			SetCamMaxSize();
			camDes = PointF(CamArea.x1, GetCamRect().y1);

			//Zoom(1.1, Input::GetMouseXY());
		default:
			break;
		}

		/*if (!GetCamRect().RectFullInside(CamArea) && !CamArea.RectFullInside(GetCamRect()))
		{
		camDes = PointF(0.0f, 0.0f);
		return;
		}*/
	}
	void ZoomNoMove(float s)
	{
		float minScale = std::min(dim.x / mWidth, dim.y / mHeight);

		scalarDes = std::max(minScale, scalarDes * s);
		scalarDes = std::min(maxCameraZoom, scalarDes);
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
	//Entity die Animation
	virtual void Event_EntityKilled(MapEntity& e) override
	{
		int numParts = int(Settings::GetDetailLevel() * 40.0f);
		if (numParts == 0)
			return;

		numParts = (rand() % numParts) + 1;

		explosionParticle p;
		p.pos = e.getPos();
		p.col = Color::GetTeamColor(e.getTeam());
		p.time = 0.0f;

		muDieEnts.Lock();
		//push back particles
		for (int i = 0; i < numParts; ++i)
		{
			p.vel = GetRandVec();
			dieEnts.push_back(p);
		}
		muDieEnts.Unlock();
	}
	//Planet capture animation
	virtual void Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity*) override
	{
		int numParts = int(Settings::GetDetailLevel() * 40.0f);
		if (numParts == 0)
			return;

		numParts = (rand() % numParts) + numParts;

		explosionParticle p;
		MapPlanet& pl = map.getPlan(pID);
		const PointF& center = pl.getPos();
		const float r2 = pl.getRadius() / 2.0f;
		p.time = 0.0f;
		p.col = Color::GetTeamColor(newTeam);

		muCapPlan.Lock();
		//push back particles
		for (int i = 0; i < numParts; ++i)
		{
			p.vel = GetRandVec();
			p.pos = center + p.vel * r2;
			capPlans.push_back(p);
		}
		muCapPlan.Unlock();
	}
	virtual void Event_EntitySpawn(PlanetID pID, MapEntity& e) override
	{
		lastPlanSpawn[pID] = 0.0f;
	}
private:
	Map& map;
	const byte myTeam;

	// effects
	std::deque< explosionParticle > dieEnts;
	Mutex muDieEnts;

	std::deque< explosionParticle > capPlans;
	Mutex muCapPlan;

	std::vector<Color> planColors;
	std::vector<float> lastPlanSpawn;

	//Positioning etc...
	PointF pos; // screen coord
	PointF dim; // screen metrics


	float scalar = 1.0f;
	float scalarDes = 1.0f;
	PointF camPos;
	PointF camDes; // desired

	//magnifiy the maxium area where the camera can be (dont make it lower then 1 (nobody knows what can happen (lol)))
	const float magnifier = 1.3f;
	const float maxCameraZoom = 2.0f;
	RectF CamArea;

	const float mWidth;
	const float mHeight;

	UIGridEntity entSelect;
	bool bEntSelectEnable = true;

	bool bFirstOrigin = true;
};
