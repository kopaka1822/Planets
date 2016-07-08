#pragma once

#include "Input.h"
#include "UILabel.h"
#include "../Utility/Mutex.h"
#include "Window.h"
#include "../System/System.h"

class UITextInput : public UIObject
{
public:
	enum FLAGS
	{
		F_NUMBER = 1,
		F_UPPERCASE = 2, // A B C
		F_LOWERCASE = 4, // a b c
		F_SPACE = 8,
		F_SPECIAL = 16, //! " $ %
		F_INETS = 32, // . - 
		F_FLOAT = F_INETS | F_NUMBER,
		F_SERVER = F_INETS | F_NUMBER | F_LOWERCASE,
		F_STRICT_NAME = 64
	};
public:
	UITextInput(Font& font, int maxLen) 
		:
		maxLen(maxLen),
		font(font)
	{
		SetMetrics({ font.GetFontHeight(), font.GetFontWidth() * maxLen });
	}
	void SetFlags(unsigned int f)
	{
		flags = f;
	}
	virtual void Draw(Drawing& draw) override
	{
		font.SetColor(Color::White());

		std::string s = GetTextDisplay();

		
		font.Text(s, pos);
	}
	virtual void Event_MouseMove(const PointF& mpos) override
	{
		if (RectF(pos, pos + dim).PointInside(mpos))
		{
			if (!bHover)
			{
				//Sound::Effect(Sound::S_HOVER);
				bHover = true;
			}
		}
		else
		{
			bHover = false;
		}
	}

	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		if (bHover && k == Input::Left)
		{
			bSelected = true;
			System::StartTextInput();
		}
		else
			bSelected = false;
	}
	virtual void Event_KeyDown(SDL_Scancode code) override
	{
		if (bSelected)
		{
			switch (code)
			{
			case SDL_SCANCODE_RETURN:
				bSelected = false;
				System::StopTextInput();
				break;
			case SDL_SCANCODE_LEFT:
				if (left.length())
				{
					muTxt.Lock();
					right = left.back() + right;
					left.pop_back();
					muTxt.Unlock();
				}
				break;
			case SDL_SCANCODE_RIGHT:
				if (right.length())
				{
					muTxt.Lock();
					left += right.front();
					right = right.substr(1, right.length());
					muTxt.Unlock();
				}
				break;
			case SDL_SCANCODE_DELETE:
				if (right.length())
				{
					muTxt.Lock();
					right = right.substr(1, right.length());
					muTxt.Unlock();
				}
				break;
			case SDL_SCANCODE_BACKSPACE:
				DeleteChar();
				break;
			case SDL_SCANCODE_C:
				// copy ?
				if (bSelected && ctrlDown)
				{
					CopyClipboard();
				}
				break;
			case SDL_SCANCODE_V:
				// paste ?
				if (bSelected && ctrlDown)
				{
					ReadClipboard();
				}
				break;
			case SDL_SCANCODE_LCTRL:
				ctrlDown = true;
				break;
			}
		}
	}
	virtual void Event_KeyUp(SDL_Scancode code) override
	{
		switch (code)
		{
		case SDL_SCANCODE_LCTRL:
			ctrlDown = false;
			break;
		}
	}
	virtual void Event_Char(char c) override
	{
		if (bSelected)
		{
			AddChar(c);
		}
	}
	bool Changed()
	{
		bool temp = bChanged;
		bChanged = false;
		return temp;
	}
	virtual void SetMetrics(const PointF& d) override
	{
		maxDisp = (unsigned int)(d.x / font.GetFontWidth() - 1);
		UIObject::SetMetrics(d);
	}
	float GetFontHeight() const
	{
		return font.GetFontHeight();
	}
	void Select()
	{
		bSelected = true;
	}
	void Deselect()
	{
		bSelected = false;
	}
	void Clear()
	{
		SetText(std::string(""));
	}
	bool isSelected() const
	{
		return bSelected;
	}
	std::string GetText() const
	{
		std::string r;
		muTxt.Lock();
		r = left + right;
		muTxt.Unlock();
		return r;
	}
	void SetText(const std::string& str)
	{
		muTxt.Lock();
		left = str;
		right = "";
		muTxt.Unlock();
	}
private:
	// paste
	void ReadClipboard()
	{
		if (System::HasClipboardText())
		{
			std::string clip = System::GetClipboardText();
			for (const auto& c : clip)
				AddChar(c);
		}
	}
	// copy to
	void CopyClipboard()
	{
		System::SetClipboardText(GetText());
	}
	void AddChar(char c)
	{
		if (GetText().length() < maxLen)
		{
			bool allowed = false;
 			if (flags & F_NUMBER && c >= '0' && c <= '9')
			{
				allowed = true;
			}

			if (flags & F_LOWERCASE && c >= 'a' && c <= 'z')
			{
				allowed = true;
			}

			if (flags & F_UPPERCASE && c >= 'A' && c <= 'Z')
			{
				allowed = true;
			}

			if (flags & F_SPACE && c == ' ')
			{
				allowed = true;
			}

			if (flags & F_INETS && (c == '.' || c == '-'))
			{
				allowed = true;
			}

			if (flags & F_SPECIAL)
			{
				if (c > 32 && c <= 47)
					allowed = true;

				if (c >= 58 && c <= 64)
					allowed = true;

				if (c >= 91 && c <= 96)
					allowed = true;

				if (c >= 123 && c <= 126)
					allowed = true;
			}

			if ((flags & F_STRICT_NAME) && allowed)
			{
				switch (c)
				{
				case '\\':
				case '/':
				case ':':
				case '*':
				case '?':
				case '"':
				case '<':
				case '>':
				case '|':
					allowed = false;
					break;
				}
			}

			if (allowed)
			{
				muTxt.Lock();
				left += c;
				muTxt.Unlock();
				bChanged = true;
			}
		}
	}
	void DeleteChar()
	{
		if (left.length())
		{
			muTxt.Lock();
			left.erase(left.end() - 1);
			muTxt.Unlock();
			bChanged = true;
		}
	}
private:
	std::string GetTextDisplay() const
	{
		static int timer = 0;

		if (bSelected)
		{
			muTxt.Lock();
			std::string res = left;
			muTxt.Unlock();

			if (timer++ > 30)
				timer = 0;

			if (timer <= 15)
				res += '|';
			else
				res += ' ';

			muTxt.Lock();
			if (res.length() + right.length() > maxDisp && right.length() > 0)
			{
				std::string rcopy = right;
				muTxt.Unlock();
				do
				{
					res += rcopy.front();
					rcopy = rcopy.substr(1, rcopy.length());
				} while (res.length() < maxDisp);
			}
			else
			{
				res += right;
				muTxt.Unlock();
			}

			while (res.length() > maxDisp)
			{
				res = res.substr(1, res.length());
			}

			return res;
		}
		else
		{
			std::string res = GetText();
			while (res.length() > maxDisp)
			{
				res.pop_back();
			}
			return res;
		}
			
	}
private:
	bool bSelected = false;
	bool bHover = false;
	bool bChanged = false;

	const unsigned int maxLen;
	unsigned int maxDisp; //number of maximal display chars with cur. dim

	unsigned int flags = ~F_STRICT_NAME;

	std::string left;
	std::string right;
	Mutex muTxt;

	Font& font;

	bool ctrlDown = false;
};