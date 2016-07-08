#pragma once
#include "UIObject.h"
#include <vector>
#include "UIScrollBar.h"

class UIItemLister : public UIObject
{
	struct Item
	{
		UIObject* left;
		UIObject* right;
	};
public:
	enum class Order
	{
		left,
		center
	};
public:
	UIItemLister(bool lines = true)
		:
		oLeft(Order::left),
		oRight(Order::left),
		drawLines(lines)
	{}
	void AddItem(UIObject* l, UIObject* r)
	{
		Item i;
		i.left = l;
		i.right = r;
		assert(l != nullptr);
		items.push_back(i);
	}
	virtual void Draw(Drawing& draw) override
	{
		if (!isEnabled())
			return;

		for (auto& itm : items)
		{
			if (itm.left->isEnabled())
			{
				itm.left->Draw(draw);
				//draw items
				if (itm.right)
					itm.right->Draw(draw);
			}
		}
		if (drawLines)
		{
			for (const auto& f : lines)
			{
				//Draw HLines
				if (f >= pos.y && f <= pos.y + dim.y)
					draw.DrawHLine(pos + PointF(0.0f, f), dim.x, 3.0f, Color::White());
			}
		}

		if (bNeedScroll)
			sbar.Draw(draw);
	}
	void SetOrder(Order l, Order r)
	{
		oLeft = l;
		oRight = r;
	}
	//Only Origins, not Metrics!
	virtual void OrderItems()
	{
		const float mid = dim.x * midpoint;
		float y = 3.0f;

		lines.clear();

		for (auto& itm : items)
		{
			float maxHei = itm.left->GetMetrics().y;
			if (itm.right)
				maxHei = std::max(maxHei, itm.right->GetMetrics().y);

			//TODO set order for middle
			float dy = (maxHei - itm.left->GetMetrics().y) / 2.0f;
			itm.left->SetOrigin(PointF(pos.x + 3.0f, pos.y + y + dy));

			if (itm.right)
			{
				dy = (maxHei - itm.right->GetMetrics().y) / 2.0f;
				itm.right->SetOrigin(PointF(pos.x + mid, pos.y + y + dy));
			}

			y += maxHei + 9.0f; // 6 for horizontal line
			lines.push_back(y - 4.5f);
		}
		maxCam = y;
		cam = 0.0f;

		// order things
		if (dim.y < maxCam)
		{
			sbar.Init(cam, dim.y, maxCam);
			sbar.SetMetrics({ 20.0f, dim.y });// right
			sbar.SetOrigin(pos + PointF(dim.x - 20.0f, 0.0f));
			bNeedScroll = true;
			sbar.Enable();
		}
		else
		{
			bNeedScroll = false;
			sbar.Disable();
		}

		
		MoveCam(0.0f);
		//assert(y <= dim.y);
	}
	void MoveMidpoint(float percent)
	{
		midpoint = percent;
	}
	void MoveCam(float d)
	{
		float nCam = std::max(0.0f, cam + d);
		nCam = std::min(nCam, maxCam - dim.y);
		nCam = std::max(nCam, 0.0f);
		float dy = cam - nCam;
		cam = nCam;
		sbar.SetCam(cam);

		//if (dy != 0.0f)
		{
			//move items
			const float my = pos.y + dim.y;
			for (auto& itm : items)
			{
				itm.left->SetOrigin(itm.left->GetOrigin() + PointF(0.0f, dy));
				
				if (itm.right)
					itm.right->SetOrigin(itm.right->GetOrigin() + PointF(0.0f, dy));

				if (itm.right)
				{
					if (itm.left->GetOrigin().y >= pos.y && itm.left->GetOrigin().y + itm.left->GetMetrics().y <= my
						&& itm.right->GetOrigin().y >= pos.y && itm.right->GetOrigin().y + itm.right->GetMetrics().y <= my)
					{
						itm.left->Enable();
						itm.right->Enable();
					}
					else
					{
						itm.left->Disable();
						itm.right->Disable();
					}
				}
				else
				{
					if (itm.left->GetOrigin().y >= pos.y && itm.left->GetOrigin().y + itm.left->GetMetrics().y <= my)
					{
						itm.left->Enable();
					}
					else
					{
						itm.left->Disable();
					}
				}
			}
			for (auto& l : lines)
			{
				l += dy;
			}
		}
	}

	virtual void Enable() override
	{
		UIObject::Enable();

		for (auto& i : items)
		{
			if (i.left)
				i.left->Enable();

			if (i.right)
				i.right->Enable();
		}
		if (bNeedScroll)
			sbar.Enable();

		CheckVisible();
		//MoveCam(0.0f);
	}
	virtual void Disable() override
	{
		UIObject::Disable();

		for (auto& i : items)
		{
			if (i.left)
				i.left->Disable();

			if (i.right)
				i.right->Disable();
		}

		sbar.Disable();
	}

	virtual void Register(GameState& gs) override
	{
		sbar.Register(gs);
		UIObject::Register(gs);
		/*for (auto& i : items)
		{
			if (i.left)
				i.left->Register(gs);

			if (i.right)
				i.right->Register(gs);
		}*/
	}

	// Event forwarding
	virtual void Event_KeyDown(SDL_Scancode code) override
	{
		for (auto& i : items)
		{
			if (i.left)
				i.left->Event_KeyDown(code);

			if (i.right)
				i.right->Event_KeyDown(code);
		}
	}
	virtual void Event_KeyUp(SDL_Scancode code) override
	{
		for (auto& i : items)
		{
			if (i.left)
				i.left->Event_KeyUp(code);

			if (i.right)
				i.right->Event_KeyUp(code);
		}
	}
	
	virtual void Event_GameKeyDown(Input::GameKey k, const PointF& p)override
	{
		for (auto& i : items)
		{
			if (i.left)
				i.left->Event_GameKeyDown(k, p);

			if (i.right)
				i.right->Event_GameKeyDown(k, p);
		}
	}
	virtual void Event_GameKeyUp(Input::GameKey k, const PointF& p)override
	{
		for (auto& i : items)
		{
			if (i.left)
				i.left->Event_GameKeyUp(k, p);

			if (i.right)
				i.right->Event_GameKeyUp(k, p);
		}
	}
	virtual void Event_Char(char code)override
	{
		for (auto& i : items)
		{
			if (i.left)
				i.left->Event_Char(code);

			if (i.right)
				i.right->Event_Char(code);
		}
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& p)override
	{
		for (auto& i : items)
		{
			if (i.left)
				i.left->Event_MouseDown(k, p);

			if (i.right)
				i.right->Event_MouseDown(k, p);
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& p)override
	{
		for (auto& i : items)
		{
			if (i.left)
				i.left->Event_MouseUp(k, p);

			if (i.right)
				i.right->Event_MouseUp(k, p);
		}
	}
	virtual void Event_MouseMove(const PointF& pos)override
	{
		for (auto& i : items)
		{
			if (i.left)
				i.left->Event_MouseMove(pos);

			if (i.right)
				i.right->Event_MouseMove(pos);
		}
	}
	void CheckVisible()
	{
		for (auto& i : items)
		{
			if (i.left)
			{
				if (GetRect().RectFullInside(i.left->GetRect()))
					i.left->Enable();
				else
					i.left->Disable();
			}

			if (i.right)
			{
				if (GetRect().RectFullInside(i.right->GetRect()))
					i.right->Enable();
				else
					i.right->Disable();
			}
		}
	}

	size_t CountItems()
	{
		return items.size();
	}

	// not thread safe!
	void Clear()
	{
		items.clear();
		lines.clear();
	}
protected:
	std::vector<Item> items;
	std::vector<float> lines;
	float midpoint = 0.5f;
	Order oLeft;
	Order oRight;

	bool drawLines;

	float cam = 0.0f;
	float maxCam = 0.0f;

	UIScrollBar sbar;
	bool bNeedScroll = false;
};