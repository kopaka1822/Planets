#pragma once
#include "UIObject.h"
#include <list>
#include "../Utility/Mutex.h"
#include "UITextInput.h"

class UIChatBox : public UIObject
{
	struct item
	{
		Color col;
		std::string text;
		unsigned long long time;
	};
public:
	UIChatBox(Font& font)
		:
		inp(font,255),
		font(font)
	{
	}
	// Set Origin first!
	virtual void setMetrics(const PointF& d) override
	{
		UIObject::setMetrics(d);

		// set inp position
		inp.setMetrics({ d.x, font.GetFontHeight() });
		inp.setOrigin(PointF(m_pos.x, m_pos.y + m_dim.y - inp.getMetrics().y));
	}
	virtual ~UIChatBox()
	{}
	virtual void draw(Drawing& draw) override
	{
		// draw 2 boxes
		//draw.DrawRect(RectF(pos, m_pos + PointF(dim.x, dim.y - inp.getMetrics().y - padding)), Color::Gray().Transparent(0.2f));

		// draw messages
		// beginning bottom
		if (bForeground)
		{
			float curpos = m_pos.y + m_dim.y - padding - inp.getMetrics().y - font.GetFontHeight();

			bool isTyping = inp.isSelected();

			muList.Lock();
			for (const auto& i : messages)
			{
				if (curpos > m_pos.y)
				{
					if (!isTyping)
						if (t.ConvertMilli(i.time) > 10000.0f)
							break; // dont disply if message is older than 10 secs

					font.SetColor(i.col);
					font.Text(i.text, PointF(m_pos.x, curpos));
					curpos -= font.GetFontHeight();
				}
				else
				{
					break;
				}
			}
			muList.Unlock();

			if (inp.isSelected())
			{
				draw.DrawRect(inp.getRect(), Color::Gray());
				inp.draw(draw);
			}
		}
	}
	void AddMessage(std::string mes, byte team)
	{
		const unsigned int maxLen = (int)(m_dim.x / font.GetFontWidth());
		item i;
		i.col = Color::GetTeamColor(team);
		i.time = Timer::GetTimestamp();

		while (mes.length())
		{
			char c = mes[0];

			if (c == '\n')
			{
				muList.Lock();
				messages.push_front(i);
				muList.Unlock();
				i.text.clear();
			}
			else
			{
				// normal char
				i.text.push_back(c);
				if (i.text.length() > maxLen)
				{
					muList.Lock();
					messages.push_front(i);
					muList.Unlock();
					i.text.clear();
				}
			}

			if (mes.length() > 1)
				mes = mes.substr(1);
			else
				break;
		}

		if (i.text.length())
		{
			muList.Lock();
			messages.push_front(i);
			muList.Unlock();
		}
		/*while (mes.length() > maxLen)
		{
			i.text = mes.substr(0, maxLen);

			muList.Lock();
			messages.push_front(i);
			muList.Unlock();

			mes = mes.substr(maxLen);
		}
		
		if (mes.length() != 0)
		{
			i.text = mes;
			muList.Lock();
			messages.push_front(i);
			muList.Unlock();
		}*/

		bChange = true;
	}
	virtual void Event_Char(char c) override
	{
		if (!ignoreNext)
			inp.Event_Char(c);
		else
			ignoreNext = false;
	}
	virtual void Event_KeyUp(SDL_Scancode code) override
	{
		inp.Event_KeyUp(code);
	}
	virtual void Event_KeyDown(SDL_Scancode code) override
	{
		if (code == SDL_SCANCODE_RETURN)
		{
			inp.Deselect();
			mes = inp.GetText();
			
			if (mes.length() > 0)
			{
				hasMessage = true;
				if (mes.length() == 2)
				{
					if (mes == "gg" || mes == "GG")
						Settings::UnlockAchievement(Settings::A_GG);
				}
			}
				
			inp.Clear();
		}
		else
		{
			inp.Event_KeyDown(code);
		}
	}
	virtual void Event_GameKeyDown(Input::GameKey k, const PointF& p) override
	{
		if (k == Input::GK_CHAT)
		{
			bForeground = true;
			if (!inp.isSelected())
			{
				inp.Select();
				ignoreNext = true;
			}
		}
	}
	bool HasMessage()
	{
		bool t = hasMessage;
		hasMessage = false;
		return t;
	}
	std::string getMessage() const
	{
		return mes;
	}
	bool HasChange()
	{
		bool t = bChange;
		bChange = false;
		return t;
	}
	void SetForeground(bool b)
	{
		bForeground = b;
		if (bForeground == false)
		{
			StopTyping();
		}
	}
	bool GetForeground() const
	{
		return bForeground;
	}
	bool isTyping() const
	{
		return inp.isSelected();
	}
	void StopTyping()
	{
		inp.Deselect();
		inp.Clear();
	}
private:
	std::list< item > messages;
	Mutex muList;
	UITextInput inp;
	Font& font;
	const float padding = 10.0f;

	bool hasMessage = false;
	std::string mes;

	bool ignoreNext = false;
	bool bChange = false;
	bool bForeground = true;

	Timer t;
};