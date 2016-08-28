#pragma once
#include "UIObject.h"
#include "../Game/MapLoader.h"

#include <stack>

class UIEditorGrid : public UIObject
{
public:
	enum class type
	{
		none,
		planet,
		entity
	};
private:
	struct Item
	{
		std::vector<MapLoader::MapPlanet> plans;
		std::vector<MapLoader::MapSpawn> spawns;
		std::vector<TeamID> teams;

		PointF size;
	};
public:
	UIEditorGrid()
		:
		cam(0.0f,0.0f)
	{
		Item i;
		i.size = { 800.0f, 600.0f };
		i.teams.assign(25, 0);
		for (size_t it = 0; it < 25; it++)
			i.teams[it] = it + 1;

		stack.push(std::move(i));
	}
	void MirrorX()
	{
		Item itm = stack.top();
		float wi = itm.size.x;
		itm.size.x *= 2.0f;
		size_t sz = itm.plans.size();
		for (size_t i = 0; i < sz; i++)
		{
			if (itm.plans.size() < 254)
			{
				float mirrx = wi + wi - itm.plans[i].x;
				if (mirrx != wi)
				{
					MapLoader::MapPlanet p = itm.plans[i];
					p.x = mirrx;
					itm.plans.push_back(p);
				}
			}
		}
		sz = itm.spawns.size();
		for (size_t i = 0; i < sz; i++)
		{
			if (itm.spawns.size() < 254)
			{
				float mirrx = wi + wi - itm.spawns[i].x;
				if (mirrx != wi)
				{
					MapLoader::MapSpawn p = itm.spawns[i];
					p.x = mirrx;
					itm.spawns.push_back(p);
				}
			}
		}
		AddItem(itm);
	}
	void MirrorY()
	{
		Item itm = stack.top();
		float wi = itm.size.y;
		itm.size.y *= 2.0f;
		size_t sz = itm.plans.size();
		for (size_t i = 0; i < sz; i++)
		{
			if (itm.plans.size() < 254)
			{
				float mirrx = wi + wi - itm.plans[i].y;
				if (mirrx != wi)
				{
					MapLoader::MapPlanet p = itm.plans[i];
					p.y = mirrx;
					itm.plans.push_back(p);
				}
			}
		}
		sz = itm.spawns.size();
		for (size_t i = 0; i < sz; i++)
		{
			if (itm.spawns.size() < 254)
			{
				float mirrx = wi + wi - itm.spawns[i].y;
				if (mirrx != wi)
				{
					MapLoader::MapSpawn p = itm.spawns[i];
					p.y = mirrx;
					itm.spawns.push_back(p);
				}
			}
		}
		AddItem(itm);
	}

	std::vector< byte > GetCurTeams() const
	{
		const Item& itm = stack.top();
		byte tms = std::max((byte)2, CountTeams(itm));

		std::vector< byte > res;
		for (byte i = 0; i < tms; i++)
		{
			res.push_back(itm.teams[i]);
		}
		return res;
	}
	void SetCurTeams(const std::vector< byte >& ts)
	{
		Item i = stack.top();
		for (size_t it = 0, end = std::min(ts.size(), i.teams.size()); it < end; it++)
		{
			i.teams[it] = ts[it];
		}
		AddItem(i);
	}

	virtual void draw(Drawing& draw) override
	{
		//draw background
		DrawGrid(draw);
		
		//draw Planets
		muStack.Lock();
		const Item itm = stack.top();
		muStack.Unlock();

		DrawPlans(draw, itm.plans);
		DrawSpawns(draw, itm.spawns);

		draw.GetFont(Drawing::F_SMALL).SetColor(Color::White());
	}
	virtual void Event_MouseWheel(float amount, const PointF& pos) override
	{
		if (amount < 0.0f)
		{
			scale = std::max(scale* 0.9f, 0.125f);
		}
		else
		{
			scale = std::min(scale * 1.1f, 2.0f);
		}
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& p) override
	{
		if ((k == Input::Middle || k == Input::Left) && getRect().PointInside(p))
		{
			bDown = true;
			lastMouse = p;
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& p) override
	{
		bDown = false;
	}
	virtual void Event_MouseMove(const PointF& p) override
	{
		if (bDown)
		{
			PointF d = lastMouse - p;
			lastMouse = p;

			cam.x -= d.x;
			cam.y -= d.y;
		}
	}

	void WidthPlus()
	{
		Item i = stack.top();
		i.size.x += 100.0f;
		AddItem(i);
	}
	void WidthOtherPlus()
	{
		Item i = stack.top();
		i.size.x += 100.0f;
		for (auto& p : i.plans)
		{
			p.x += 100.0f;
		}
		for (auto& s : i.spawns)
		{
			s.x += 100.0f;
		}
		AddItem(i);
		cam.x -= 100.0f * GetScalar();
	}
	void WidthOtherMinus()
	{
		Item i = stack.top();
		i.size.x -= 100.0f;
		if (i.size.x <= 400.0f)
			return;

		for (auto& p : i.plans)
		{
			p.x -= 100.0f;
		}
		for (auto& s : i.spawns)
		{
			s.x -= 100.0f;
		}
		AddItem(i);
		cam.x += 100.0f * GetScalar();
	}
	void HeightOtherPlus()
	{
		Item i = stack.top();
		i.size.y += 100.0f;
		for (auto& p : i.plans)
		{
			p.y += 100.0f;
		}
		for (auto& s : i.spawns)
		{
			s.y += 100.0f;
		}
		AddItem(i);
		cam.y -= 100.0f * GetScalar();
	}
	void HeightOtherMinus()
	{
		Item i = stack.top();
		i.size.y -= 100.0f;
		if (i.size.y <= 400.0f)
			return;

		for (auto& p : i.plans)
		{
			p.y -= 100.0f;
		}
		for (auto& s : i.spawns)
		{
			s.y -= 100.0f;
		}
		AddItem(i);
		cam.y += 100.0f * GetScalar();
	}
	void WidthMinus()
	{
		Item i = stack.top();
		i.size.x -= 100.0f;
		if (i.size.x > 400.0f)
			AddItem(i);
	}
	void HeightPlus()
	{
		Item i = stack.top();
		i.size.y += 100.0f;
		AddItem(i);
	}
	void HeightMinus()
	{
		Item i = stack.top();
		i.size.y -= 100.0f;
		if(i.size.y > 400.0f)
			AddItem(i);
	}
	float GetScalar() const
	{
		return scale;
	}

	void AddPlanet(MapLoader::MapPlanet p)
	{
		Item i = stack.top();

		if (i.plans.size() > 254)
			return; // max planet size

		auto pos = MouseToModel();
		p.x = pos.x;
		p.y = pos.y;
		i.plans.push_back(p);
		AddItem(i);
	}
	void ChangeTeams(const std::vector< byte >& ts)
	{
		Item i = stack.top();
		bool bChange = false;
		for (size_t it = 0, end = std::min(ts.size(), i.teams.size()); it < end; it++)
		{
			if (i.teams[it] != ts[it])
			{
				bChange = true;
			}
			i.teams[it] = ts[it];
		}
		if (bChange)
		{
			AddItem(i);
		}
	}
	void ChangePlanet(MapLoader::MapPlanet plan, int id)
	{
		Item i = stack.top();
		for (auto& p : i.plans)
		{
			if (id == 0)
			{
				// this is the planet
				p = plan;
				break;
			}
			else if (id < 0)
				return;
			id--;
		}

		AddItem(i);
	}
	void ChangeSpawn(MapLoader::MapSpawn spaw, int id)
	{
		Item i = stack.top();
		for (auto& s : i.spawns)
		{
			if (id == 0)
			{
				// this is the planet
				s = spaw;
				break;
			}
			else if (id < 0)
				return;
			id--;
		}

		AddItem(i);
	}
	void AdjustPlans()
	{
		Item i = stack.top();
		for (auto& p : i.plans)
		{
			MapLoader::AdjustMapPlanet(&p);
		}
		AddItem(i);
	}
	void AddSpawn(MapLoader::MapSpawn s)
	{
		Item i = stack.top();
		auto pos = MouseToModel();
		s.x = pos.x;
		s.y = pos.y;
		i.spawns.push_back(s);
		AddItem(i);
	}
	//align to 100 200 300...
	void AlignLine()
	{
		std::function<int(int)> align = [](int x)
		{
			int m = x % 100;
			if (m > 50)
				return x + 100 - m;
			else
				return x - m;
		};

		Item i = stack.top();
		for (auto& p : i.plans)
		{
			p.x = (float)align((int)p.x);
			p.y = (float)align((int)p.y);
		}
		for (auto& s : i.spawns)
		{
			s.x = (float)align((int)s.x);
			s.y = (float)align((int)s.y);
		}
		AddItem(i);
	}
	//Align to 50 150 250
	void AlignMid()
	{
		std::function<int(int)> align = [](int x)
		{
			int m = x % 100;
			if (m > 50)
				return x + 50 - m;
			else
				return x - m + 50;
		};

		Item i = stack.top();
		for (auto& p : i.plans)
		{
			p.x = (float)align((int)p.x);
			p.y = (float)align((int)p.y);
		}
		for (auto& s : i.spawns)
		{
			s.x = (float)align((int)s.x);
			s.y = (float)align((int)s.y);
		}
		AddItem(i);
	}


	void Undo()
	{
		if (stack.size() > 1)
		{
			muStack.Lock();
			redo.push(stack.top());
			stack.pop();
			muStack.Unlock();
		}
	}
	void Redo()
	{
		if (!redo.empty())
		{
			muStack.Lock();
			stack.push(redo.top());
			muStack.Unlock();
			redo.pop();
		}
	}

	void DeleteAtMouse()
	{
		const PointF m = MouseToModel();

		Item itm = stack.top();

		bool match = false;
		for (auto s = itm.spawns.begin(), end = itm.spawns.end(); s != end; ++s)
		{
			if ((PointF(s->x, s->y) - m).lengthSq() < 250.0f)
			{
				//delete this
				itm.spawns.erase(s);

				match = true;
				break;
			}
		}

		if (!match)
		{
			for (auto p = itm.plans.begin(), end = itm.plans.end(); p != end; ++p)
			{
				if ((PointF(p->x, p->y) - m).lengthSq() <= p->radius * p->radius)
				{
					itm.plans.erase(p);

					match = true;
					break;
				}
			}
		}

		if (match)
		{
			AddItem(itm);
		}
	}

	void Reset()
	{
		muStack.Lock();
		assert(stack.size() != 0);
		while (stack.size() != 1)
		{
			stack.pop();
		}
		muStack.Unlock();
		while (!redo.empty())
		{
			redo.pop();
		}
		scale = 1.0f;
		cam = { 0.0f, 0.0f };
	}

	PointF MouseToModel() const
	{
		PointF ms = Input::GetMouseXY();
		ms = ms - cam - m_pos;
		return ms / scale;
	}

	// returns the id of the selected object and fills pType with the appropriate type
	int GetSelected(type* pType) const
	{
		const Item& itm = stack.top();
		const PointF m = MouseToModel();

		int id = 0;
		*pType = type::entity;
		for (const auto& s : itm.spawns)
		{
			if ((PointF(s.x, s.y) - m).lengthSq() < 250.0f)
			{
				return id;
			}
			id++;
		}

		id = 0;
		*pType = type::planet;
		for (const auto& p : itm.plans)
		{
			if ((PointF(p.x, p.y) - m).lengthSq() <= p.radius * p.radius)
			{
				return id;
			}
			id++;
		}

		*pType = type::none;
		return -1;
	}
	const MapLoader::MapPlanet* GetPlanByID(int id)
	{
		const auto& itms = stack.top();
		for (const auto& i : itms.plans)
		{
			if (id == 0)
			{
				return &i;
			}
			id--;
		}
		return nullptr;
	}
	const MapLoader::MapSpawn* GetEntByID(int id)
	{
		const auto& itms = stack.top();
		for (const auto& i : itms.spawns)
		{
			if (id == 0)
			{
				return &i;
			}
			id--;
		}
		return nullptr;
	}

	bool MapIsValid(std::string* dstError) const
	{
		assert(dstError != nullptr);
		bool error = false;

		//Check if planets and spawns are inside
		const Item& itm = stack.top();
		const RectF g = RectF({ 0.0f, 0.0f },itm.size);

		for (const auto& p : itm.plans)
		{
			if (!g.RectFullInside(RectF::FromPoint({ p.x, p.y }, p.radius)))
			{
				*dstError = "planet not in grid";
				return false;
			}
		}
		for (const auto& s : itm.spawns)
		{
			if (!g.PointInside({ s.x, s.y }))
			{
				*dstError = "spawn not in grid";
				return false;
			}
		}
		//Check teams
		std::vector<bool> teams;
		teams.resize(25, false);

		for (const auto& p : itm.plans)
		{
			teams[p.team] = true;
		}
		for (const auto& s : itm.spawns)
		{
			teams[s.team] = true;
		}
		teams[0] = false;
		int teamCount = 0;
		//count teams
		for (const auto& t : teams)
		{
			if (t)
				teamCount++;
		}

		//check for errors (e.g. m_team 1-3 and 5 exist, but m_team 4 not...)
		int count2 = -1; // skip the first one
		for (const auto& t : teams)
		{
			if (count2 == -1)
				count2++;
			else
			{
				if (t)
					count2++;
				else
					break;
			}
		}

		if (teamCount < 2)
		{
			*dstError = "not enough players";
			return false;
		}
		if (count2 != teamCount)
		{
			*dstError = "skipped player";
			return false;
		}

		// all in same m_team?
		byte startTeam = itm.teams[0];
		bool bDiffTeams = false;
		for (int i = 1; i < teamCount; i++)
		{
			if (itm.teams[i] != startTeam)
			{
				bDiffTeams = true;
				break;
			}
		}

		if (!bDiffTeams)
		{
			*dstError = "all players in same m_team";
			return false;
		}
		

		return true;
	}
	bool SaveMap(const std::string& filen) const
	{
		//assuming map is valid
		const Item& itm = stack.top();

		byte teamCount = CountTeams(itm);
		/*bool bCustomTeams = false;
		for (auto i = 0; i < teamCount; i++)
		{
			if (itm.teams[i] != i + 1)
			{
				bCustomTeams = true;
				break;
			}
		}
		std::vector< byte > cTeams;
		if (bCustomTeams)
			cTeams = itm.teams;
*/
		return MapLoader::SaveMap(filen, itm.plans, itm.spawns, teamCount,itm.size.x,itm.size.y,itm.teams);
	}
	void LoadMap(const std::vector< MapLoader::MapPlanet>& plans, const std::vector< MapLoader::MapSpawn >& spawns,
		float width, float height, const std::vector< TeamID >& teams)
	{
		Reset();
		Item i;
		i.size = PointF(width, height);
		i.plans = plans;
		i.spawns = spawns;
		i.teams = stack.top().teams;
		assert(i.teams.size() == 25);
		for (size_t it = 0, end = std::min(i.teams.size(), teams.size()); it < end; it++)
		{
			i.teams[it] = teams[it];
		}

		AddItem(i);
	}
private:
	void AddItem(Item& i)
	{
		muStack.Lock();
		stack.push(std::move(i));
		muStack.Unlock();

		//delete redo things
		while (!redo.empty())
		{
			redo.pop();
		}
	}
	
	void DrawPlans(Drawing& draw,const std::vector< MapLoader::MapPlanet >& plans)
	{
		for (const auto& p : plans)
		{
			PointF coord = m_pos + cam + (PointF(p.x, p.y) * scale);
			draw.DrawPlanet(coord, Color::GetTeamColor(p.team), p.radius * scale);
		}
	}
	void DrawSpawns(Drawing& draw, const std::vector<MapLoader::MapSpawn>& spawn)
	{
		for (const auto& s : spawn)
		{
			PointF coord = m_pos + cam + (PointF(s.x, s.y) * scale);
			draw.DrawRect(RectF::FromPoint(coord, 20.0f * scale), Color::GetTeamColor(s.team));
		}
	}
private:
	void DrawGrid(Drawing& draw) const
	{
		muStack.Lock();
		assert(stack.size());
		const Item itm = stack.top();
		muStack.Unlock();
		const float d = 100.0f * scale;
		const float width = itm.size.x * scale + 0.01f;
		const float height = itm.size.y * scale + 0.01f;
		const PointF start(m_pos + cam);

		for (float x = 0.0f; x <= width; x += d)
		{
			draw.DrawVLine(start + PointF(x, 0), height, 3.0f, Color(0, 0, 200));
		}
		for (float y = 0.0f; y <= height; y += d)
		{
			draw.DrawHLine(start + PointF(0, y), width, 3.0f, Color(0, 0, 200));
		}
	}
	byte CountTeams(const Item& itm) const
	{
		byte maxTeam = 0;
		for (const auto& p : itm.plans)
			maxTeam = std::max(maxTeam, p.team);

		for (const auto& s : itm.spawns)
			maxTeam = std::max(maxTeam, s.team);

		return maxTeam;
	}
private:
	PointF cam;
	float scale = 1.0f;

	std::stack< Item > stack;
	std::stack< Item > redo;

	bool bDown = false;
	PointF lastMouse;

	Mutex muStack;
};