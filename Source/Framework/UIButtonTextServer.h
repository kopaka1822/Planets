#pragma once
#include"UIButtonText.h"

class UIButtonTextServer : public UIButtonText
{
public:
	UIButtonTextServer(const std::string& t, int id, Font& font)
		:
		UIButtonText(t,font),
		ID(id)
	{}
	virtual ~UIButtonTextServer()
	{}
	int GetID() const
	{
		return ID;
	}
private:
	const int ID;
};