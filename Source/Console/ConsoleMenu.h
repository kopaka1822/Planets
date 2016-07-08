#pragma once
#include "Console.h"
#include <vector>
#include <assert.h>

class ConsoleMenu
{
	struct Item
	{
		std::string txt;
		int code;
	};
public:
	ConsoleMenu(const std::string title)
		:
		title(title)
	{}
	void AddLine(const std::string& title, int retVal)
	{
		Item i;
		i.txt = title;
		i.code = retVal;

		itms.push_back(i);
	}

	// returns code of selected item
	int Display() const
	{
		assert(itms.size());

		Console::EndLine();
		Console::Write("--- ");
		Console::Write(title);
		Console::WriteLine(" ---");
		Console::EndLine();

		// Items
		int i = 1;
		for (const auto& e : itms)
		{
			Console::Write("\t");
			Console::Write(i++);
			Console::Write(":\t");
			Console::WriteLine(e.txt);
		}

		// try to get a respone
		bool bFinished = false;
		int retVal = 0;
		do
		{
			retVal = Console::ReadInt();
			if (retVal >= 1 && retVal <= int(itms.size()))
			{
				retVal = itms[retVal - 1].code;
				bFinished = true;
			}

		} while (!bFinished);
		
		return retVal;
	}
private:
	const std::string title;
	std::vector< Item > itms;
};