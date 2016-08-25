#pragma once
#include "UIObject.h"
#include "../Utility/Mutex.h"
#include <memory>

class UIInfoLister : public UIObject
{
public:
	struct Item
	{
		UIObject* pObj;
		PointF pos;
	};
private:
	struct Section
	{
		Section(std::vector<Item>&& i, float height)
			:
			itms(i),
			hei(height)
		{}

		std::vector<Item> itms;
		float hei;
	};

public:
	UIInfoLister()
	{

	}
	virtual ~UIInfoLister()
	{}
	virtual void draw(Drawing& draw) override
	{
		muItm.Lock();

		draw.DrawBox(getRect(), (float)border, Color::White(), Color::Black());

		float y = m_pos.y - cam + border + padding;
		const float miny = m_pos.y + border;
		const float maxy = m_pos.y + m_dim.y - border;
		const float xStart = m_pos.x + border;
		const float xEnd = m_pos.x + m_dim.x - border;
		
		for (auto& s : itms)
		{
			if (y >= miny && y <= maxy)
			{
				//draw Line
				draw.DrawRect(RectF({ xStart, y }, { xEnd, y + padding }), Color::White());
			}
			for (auto& i : s->itms)
			{
				if (i.pObj->isEnabled())
					i.pObj->draw(draw);
			}
			y += s->hei + padding;

			if (y >= miny && y <= maxy)
			{
				//draw Line
				draw.DrawRect(RectF({ xStart, y }, { xEnd, y + padding }), Color::White());
			}
			y += 2 * padding;

			if (y >= maxy)
				break;
		}


		muItm.Unlock();
	}
	void AddItem(std::vector<Item>&& itm, float height)
	{
		muItm.Lock();
		itms.push_back(std::unique_ptr<Section>(new Section(std::move(itm),height)));
		OrderItems();
		muItm.Unlock();
	}
	void DeleteItem(unsigned int index)
	{
		muItm.Lock();
		auto i = itms.begin();
		for (unsigned int in = 0; in < index; in++, i++);
		this->itms.erase(i);
		OrderItems();
		muItm.Unlock();
	}
	void OrderItems()
	{
		const float x = m_pos.x + border;
		float y = m_pos.y + border + padding + padding;

		muItm.Lock();
		for (auto& s : itms)
		{
			const PointF off(x, y);
			for (auto& i : s->itms)
			{
				i.pObj->setOrigin(i.pos + off);
			}
			y += s->hei + 3 * padding;
		}

		maxCam = y - padding;
		float c = cam;
		cam = 0.0f;
		MoveCam(c);

		muItm.Unlock();
	}
	void MoveCam(float d)
	{
		float nCam = std::max(0.0f, cam + d);
		nCam = std::min(nCam, maxCam - m_dim.y);
		nCam = std::max(nCam, 0.0f);
		float dy = cam - nCam;
		cam = nCam;
		//move items
		const PointF off(0.0f, dy);
		const float miny = m_pos.y + border;
		const float my = m_pos.y + m_dim.y - border;
		for (auto& s : itms)
		{
			for (auto& i : s->itms)
			{
				i.pObj->setOrigin(i.pObj->getOrigin() + off);

				//inside box?
				if (i.pObj->getOrigin().y >= miny &&
					i.pObj->getOrigin().y + i.pObj->getMetrics().y <= my)
				{
					i.pObj->enable();
				}
				else
				{
					i.pObj->disable();
				}
			}
		}
	}
protected:
	static const int border = 6;
	static const int padding = 4;
	void Clear()
	{
		muItm.Lock();
		itms.clear();
		muItm.Unlock();
	}
private:
	std::vector< std::unique_ptr<Section> > itms;
	Mutex muItm;

	float cam = 0.0f;
	float maxCam = 0.0f;
};