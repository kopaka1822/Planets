#pragma once
#include "UIItemListerScroll.h"
#include "UIButtonTexture.h"
#include "UIListPlanet.h"
#include "UIListEntity.h"

class UIButtonPlanSelect : public UIButton
{
public:
	UIButtonPlanSelect()
	{}
	void SetSelected(bool b)
	{
		bSelected = b;
	}
	bool isSelected()
	{
		return bSelected;
	}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawPlanet(GetRect().Midpoint(), bSelected? Color::Red() : Color::White(), 
			std::min(dim.x, dim.y) / 2.0f);
	}
private:
	bool bSelected = false;
};

class UIButtonTexSelect : public UIButtonTexture
{
public:
	UIButtonTexSelect(const Texture& tex)
		:
		UIButtonTexture(tex)
	{}
	virtual void Draw(Drawing& draw) override
	{
		if (!bSelected)
		{
			UIButtonTexture::DrawColoredBox(draw, Color::White());
		}
		else
		{
			UIButtonTexture::DrawColoredBox(draw, Color::Red());
		}
	}
	void SetSelected(bool b)
	{
		bSelected = b;
	}
	bool isSelected()
	{
		return bSelected;
	}
private:
	bool bSelected = false;
};

class UIEditorList : public UIObject
{
public:
	enum class selection
	{
		none,
		planet,
		entity
	};
	enum class state
	{
		none,
		floating,
		fixed
	};
public:
	UIEditorList(UIListPlanet& left, UIListEntity& right)
		:
		left(left),
		right(right),
		btnEnt(Database::GetTexture(Database::Particle))
	{}
	virtual ~UIEditorList()
	{}
	virtual void Register(GameState& gs) override
	{
		btnPlan.Register(gs);
		btnEnt.Register(gs);

		UIObject::Register(gs);

		left.Register(gs);
		right.Register(gs);
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& p) override
	{
		if (btnPlan.Pushed())
		{
			EnablePlans();

			curSelect = selection::planet;
			curState = state::floating;

			left.SetFloating();
		}

		if (btnEnt.Pushed())
		{
			EnableEnts();

			curSelect = selection::entity;
			curState = state::floating;

			right.SetFloating();
		}

		if (k == Input::Right)
		{
			Deselect();
		}
	}
	state GetState() const
	{
		return curState;
	}
	selection GetSelection() const
	{
		return curSelect;
	}
	MapLoader::MapPlanet GetPlanet() const
	{
		return left.GetStruct();
	}
	MapLoader::MapSpawn GetSpawn() const
	{
		return right.GetStruct();
	}
	int GetID() const
	{
		return selID;
	}
	void SelectPlanet(const MapLoader::MapPlanet& p, int id)
	{
		selID = id;
		curState = state::fixed;
		curSelect = selection::planet;
		EnablePlans();
		left.SetFixed();
		left.LoadStruct(p);
	}
	void SelectSpawn(const MapLoader::MapSpawn& s, int id)
	{
		selID = id;
		curState = state::fixed;
		curSelect = selection::entity;
		EnableEnts();
		right.SetFixed();
		right.LoadStruct(s);
	}

	// call origin first!
	virtual void SetMetrics(const PointF& m) override
	{
		UIObject::SetMetrics(m);
		PointF off = PointF(border, border) + pos;
		const float wi2 = (m.x - 3 * border) / 2.0f;
		btnPlan.SetOrigin(off);
		btnPlan.SetMetrics({ wi2, hei });

		off.x += wi2 + border;
		btnEnt.SetOrigin(off);
		btnEnt.SetMetrics({ wi2, hei });

		off = PointF(border, 2 * border + hei) + pos + PointF(border,border);
		left.SetOrigin(off);
		right.SetOrigin(off);

		float width = dim.x - 4 * border;
		float height = dim.y - 5 * border - hei;
		left.SetMetrics({ width, height });
		right.SetMetrics({ width, height });

		left.OrderItems();
		right.OrderItems();
	}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawBox(GetRect(), (float)border, Color::White(), Color::Black());

		btnPlan.Draw(draw);
		btnEnt.Draw(draw);

		draw.DrawHLine(pos + PointF(0.0f,2.5f * border + hei), dim.x, (float)border, Color::White());
		draw.DrawVLine(pos + PointF(dim.x / 2.0f, 0.0f), 2 * border + hei, (float)border, Color::White());

		if (btnPlan.isSelected())
			left.Draw(draw);

		if (btnEnt.isSelected())
			right.Draw(draw);
	}

	bool Changed()
	{
		if (selection::entity == curSelect)
		{
			return right.Changed();
		}
		else if (selection::planet == curSelect)
		{
			return left.Changed();
		}

		return false;
	}
	void SetGridMouse(const PointF& p)
	{
		if (curState == state::floating)
		{
			if (curSelect == selection::entity)
			{
				right.SetXY(p);
			}
			else if (curSelect == selection::planet)
			{
				left.SetXY(p);
			}
		}
	}
	void Deselect()
	{
		// deselct
		btnPlan.SetSelected(false);
		btnEnt.SetSelected(false);

		left.Disable();
		right.Disable();

		curSelect = selection::none;
		curState = state::none;
	}
private:
	void EnablePlans()
	{
		btnPlan.SetSelected(true);
		btnEnt.SetSelected(false);

		left.Enable();
		right.Disable();
	}
	void EnableEnts()
	{
		btnEnt.SetSelected(true);
		btnPlan.SetSelected(false);

		left.Disable();
		right.Enable();
	}
private:
	UIListPlanet& left;
	UIListEntity& right;

	UIButtonPlanSelect btnPlan;
	UIButtonTexSelect btnEnt;

	static const int border = 6;
	const float hei = 50.0f; // height of buttons

	state curState = state::none;
	selection curSelect = selection::none;
	int selID = -1;
};