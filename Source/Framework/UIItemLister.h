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
	virtual void draw(Drawing& draw) override
	{
		if (!isEnabled())
			return;

		for (auto& itm : items)
		{
			if (itm.left->isEnabled())
			{
				itm.left->draw(draw);
				//draw items
				if (itm.right)
					itm.right->draw(draw);
			}
		}
		if (drawLines)
		{
			for (const auto& f : lines)
			{
				//draw HLines
				if (f >= m_pos.y && f <= m_pos.y + m_dim.y)
					draw.DrawHLine(m_pos + PointF(0.0f, f), m_dim.x, 3.0f, Color::White());
			}
		}

		if (bNeedScroll)
			sbar.draw(draw);
	}
	void SetOrder(Order l, Order r)
	{
		oLeft = l;
		oRight = r;
	}
	//Only Origins, not Metrics!
	virtual void OrderItems()
	{
		const float mid = m_dim.x * midpoint;
		float y = 3.0f;

		lines.clear();

		for (auto& itm : items)
		{
			float maxHei = itm.left->getMetrics().y;
			if (itm.right)
				maxHei = std::max(maxHei, itm.right->getMetrics().y);

			//TODO set order for middle
			float dy = (maxHei - itm.left->getMetrics().y) / 2.0f;
			itm.left->setOrigin(PointF(m_pos.x + 3.0f, m_pos.y + y + dy));

			if (itm.right)
			{
				dy = (maxHei - itm.right->getMetrics().y) / 2.0f;
				itm.right->setOrigin(PointF(m_pos.x + mid, m_pos.y + y + dy));
			}

			y += maxHei + 9.0f; // 6 for horizontal line
			lines.push_back(y - 4.5f);
		}
		maxCam = y;
		cam = 0.0f;

		// order things
		if (m_dim.y < maxCam)
		{
			sbar.Init(cam, m_dim.y, maxCam);
			sbar.setMetrics({ 20.0f, m_dim.y });// right
			sbar.setOrigin(m_pos + PointF(m_dim.x - 20.0f, 0.0f));
			bNeedScroll = true;
			sbar.enable();
		}
		else
		{
			bNeedScroll = false;
			sbar.disable();
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
		nCam = std::min(nCam, maxCam - m_dim.y);
		nCam = std::max(nCam, 0.0f);
		float dy = cam - nCam;
		cam = nCam;
		sbar.SetCam(cam);

		//if (dy != 0.0f)
		{
			//move items
			const float my = m_pos.y + m_dim.y;
			for (auto& itm : items)
			{
				itm.left->setOrigin(itm.left->getOrigin() + PointF(0.0f, dy));
				
				if (itm.right)
					itm.right->setOrigin(itm.right->getOrigin() + PointF(0.0f, dy));

				if (itm.right)
				{
					if (itm.left->getOrigin().y >= m_pos.y && itm.left->getOrigin().y + itm.left->getMetrics().y <= my
						&& itm.right->getOrigin().y >= m_pos.y && itm.right->getOrigin().y + itm.right->getMetrics().y <= my)
					{
						itm.left->enable();
						itm.right->enable();
					}
					else
					{
						itm.left->disable();
						itm.right->disable();
					}
				}
				else
				{
					if (itm.left->getOrigin().y >= m_pos.y && itm.left->getOrigin().y + itm.left->getMetrics().y <= my)
					{
						itm.left->enable();
					}
					else
					{
						itm.left->disable();
					}
				}
			}
			for (auto& l : lines)
			{
				l += dy;
			}
		}
	}

	virtual void enable() override
	{
		UIObject::enable();

		for (auto& i : items)
		{
			if (i.left)
				i.left->enable();

			if (i.right)
				i.right->enable();
		}
		if (bNeedScroll)
			sbar.enable();

		CheckVisible();
		//MoveCam(0.0f);
	}
	virtual void disable() override
	{
		UIObject::disable();

		for (auto& i : items)
		{
			if (i.left)
				i.left->disable();

			if (i.right)
				i.right->disable();
		}

		sbar.disable();
	}

	virtual void registerMe(GameState& gs) override
	{
		sbar.registerMe(gs);
		UIObject::registerMe(gs);
		/*for (auto& i : items)
		{
			if (i.left)
				i.left->registerMe(gs);

			if (i.right)
				i.right->registerMe(gs);
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
				if (getRect().RectFullInside(i.left->getRect()))
					i.left->enable();
				else
					i.left->disable();
			}

			if (i.right)
			{
				if (getRect().RectFullInside(i.right->getRect()))
					i.right->enable();
				else
					i.right->disable();
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