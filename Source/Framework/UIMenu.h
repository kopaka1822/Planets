#pragma once
#include "UIObject.h"
#include <functional>
#include "UIButtonCross.h"

class UIMenu : public UIObject
{
	friend class Section;
public:
	static const int border = 7;
	static const int padd = 10;
protected:
	class Section : public UIObject
	{
		friend class Element;
	public:
		class Element : public UIObject
		{
		public:
			Element(std::string name, Font& font, Section* sec)
				:
				name(name),
				font(font),
				parent(sec)
			{
				m_dim = { font.GetFontWidth() * name.length() + 2 * padd, 0 };
			}
			virtual void draw(Drawing& draw) override
			{
				if (bHover)
				{
					draw.DrawRect(getRect(), Color::Gray());
				}
				font.Text(name, m_pos + PointF(padd, padd));
			}
			virtual void Event_MouseMove(const PointF& pos) override
			{
				if (getRect().PointInside(pos))
				{
					if (!bHover)
					{
						Sound::Effect(Sound::S_HOVER);
						bHover = true;
					}
				}
				else
				{
					bHover = false;
				}
			}
			virtual void Event_MouseUp(Input::MouseKey k, const PointF& p) override
			{
				bHover = getRect().PointInside(p);
				if (bHover) //Click
				{
					parent->ElmPressed(name);
					Sound::Effect(Sound::S_CLICK);
				}
			}
			void Reset()
			{
				bHover = false;
			}
		private:
			const std::string name;
			Font& font;
			bool bHover = false;

			Section* const parent = nullptr;
		};
	public:
		Section(std::string name, Font& font, UIMenu* par)
			:
			name(name),
			font(font),
			parent(par)
		{
			assert(name.length() != 0);
			//own dim
			m_dim = PointF(name.length() * font.GetFontWidth() + 2 * padd, font.GetFontHeight() + 2 * padd);

			//elm dim
			dim2 = { 0.0f, 0.0f };
		}
		virtual ~Section()
		{}
		virtual void setOrigin(const PointF& p) override
		{
			UIObject::setOrigin(p);
			pos2 = p + PointF(-border, m_dim.y + border);

			OrderElements();
		}
		virtual void draw(Drawing& draw) override
		{
			if (bHover)
				draw.DrawRect(getRect(), Color::Gray());
			font.SetColor(Color::White());
			font.Text(name, m_pos + PointI(padd,padd));

			if (bActive)
			{
				draw.DrawRect(RectF(pos2, pos2 + dim2), Color::White());
				draw.DrawRect(RectF(pos2 + PointF(border, 0), pos2 + dim2 - PointF(border, border)), Color::Black());

				for (auto& e : elms)
				{
					e->draw(draw);
				}
			}
		}
		virtual void Event_MouseMove(const PointF& p) override
		{
			if (getRect().PointInside(p))
			{
				if (!bHover)
				{
					Sound::Effect(Sound::S_HOVER);
					bHover = true;
				}
			}
			else
			{
				bHover = false;
			}

			if (bActive)
			{
				for (auto& e : elms)
				{
					e->Event_MouseMove(p);
				}
			}
		}
		virtual void Event_MouseDown(Input::MouseKey k,const PointF& p) override
		{
			//key ist always left
			if (!bActive)
			{
				bActive = getRect().PointInside(p);
				if (bActive) Sound::Effect(Sound::S_CLICK);
			}
			else
			{
				bActive = (getRect().PointInside(p)) || RectF(pos2, pos2 + dim2).PointInside(p);
			}
		}
		void AddElement(std::string name)
		{
			elms.push_back(std::unique_ptr<Element>(new Element(name,font,this)));

			OrderElements();
		}
		void OrderElements()
		{
			if (!elms.size())
				return;
			//reserach element lenght
			int len = name.length() * int(font.GetFontWidth()) + 2 * padd;
			for (const auto& e : elms)
			{
				len = std::max(len, (int)e->getMetrics().x);
			}

			PointF start = pos2 + PointF(border, 0);

			for (auto& e : elms)
			{
				e->setMetrics({ (float)len, font.GetFontHeight() + 2 * padd});
				e->setOrigin(start);

				start += PointF(0.0f, font.GetFontHeight() + 2 * padd);
			}

			//set elm dim
			dim2 = PointF(2 * border + len, border + elms.size() * (2 * padd + font.GetFontHeight()));
		}
		const std::string& GetName() const
		{
			return name;
		}
		virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
		{
			if (bActive)
			{
				for (auto& e : elms)
				{
					e->Event_MouseUp(k, pos);
				}
			}
		}
		bool isActive() const
		{
			return bActive;
		}
	private:
		void ElmPressed(const std::string& e)
		{
			parent->ElemPress(this->name, e);
			Reset();
		}
		void Reset()
		{
			bActive = false;
			for (auto& e : elms)
			{
				e->Reset();
			}
		}
	private:
		const std::string name;
		std::vector< std::unique_ptr<Element> > elms;
		Font& font;

		PointF dim2;
		PointF pos2;

		bool bHover = false;
		bool bActive = false;

		static const int border = UIMenu::border;
		static const int padd = UIMenu::padd;

		UIMenu* parent = nullptr;
	};
public:
	UIMenu(void(*pCall)(const std::string&, const std::string&))
		:
		font(Drawing::GetDraw().GetFont(Drawing::F_SMALL)),
		elHei(font.GetFontHeight()),
		Callback(pCall)
	{

	}
	virtual void setMetrics(const PointF& m) override
	{
		m_dim.x = m.x;
		m_dim.y = 2 * padd + border + font.GetFontHeight();

		btnCross.setMetrics({ m_dim.y, m_dim.y });
	}
	virtual ~UIMenu()
	{}
	void AddSection(const std::string& name)
	{
		Sects.push_back(std::unique_ptr<Section>(new Section(name, font, this)));

		OrderElements();
	}
	void AddElement(const std::string& section, const std::string& name)
	{
		for (auto& s : Sects)
		{
			if (s->GetName() == section)
			{
				s->AddElement(name);
				break;
			}
		}
	}
	virtual void setOrigin(const PointF& p) override
	{
		UIObject::setOrigin(p);
		OrderElements();

		btnCross.setOrigin(m_pos + PointF(m_dim.x - m_dim.y, 0.0f));
	}
	virtual void draw(Drawing& draw) override
	{
		float dy = 2 * padd + font.GetFontHeight();
		draw.DrawRect(RectF(m_pos, m_pos + PointF(m_dim.x, dy)), Color::Black());
		draw.DrawRect(RectF(m_pos + PointF(0, dy), m_pos + PointF(m_dim.x, dy + border)), Color::White());


		PointF curP = m_pos;
		for (auto& s : Sects)
		{
			s->draw(draw);
			//draw border
			curP += PointF(s->getMetrics().x, 0);
			draw.DrawRect(RectF(curP, curP + PointF(border, dy)), Color::White());
			curP += PointF(border, 0);
		}
		//erase last border
		curP -= PointF(border, 0);
		draw.DrawRect(RectF(curP, curP + PointF(border, dy)), Color::Black());

		btnCross.draw(draw);
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		if (k != Input::Left) return;
		for (auto& s : Sects)
		{
			s->Event_MouseDown(k, pos);
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		if (k != Input::Left) return;
		for (auto& s : Sects)
		{
			s->Event_MouseUp(k, pos);
		}
	}
	virtual void Event_MouseMove(const PointF& p) override
	{
		for (auto& s : Sects)
		{
			s->Event_MouseMove(p);
		}
	}
	bool isActive() const
	{
		for (auto& s : Sects)
		{
			if (s->isActive())
				return true;
		}
		return false;
	}
	virtual void registerMe(GameState& gs) override
	{
		UIObject::registerMe(gs);
		btnCross.registerMe(gs);
	}
	bool isClosed()
	{
		return btnCross.Pushed();
	}
private:
	void OrderElements()
	{
		PointF start = m_pos;

		for (auto& s : Sects)
		{
			s->setOrigin(start);

			start += PointF(s->getMetrics().x + border,0);
		}
	}
	void ElemPress(std::string sec, std::string elm)
	{
		Callback(sec, elm);
	}
private:
	std::vector< std::unique_ptr<Section> > Sects;
	Font& font;
	const float elHei; // Element height

	void(* Callback)(const std::string&, const std::string&) = nullptr; //section & name

	UIButtonCross btnCross;
};
