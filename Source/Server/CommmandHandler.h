#pragma once
#include <list>
#include <vector>
#include <map>
#include <assert.h>
#include <functional>

template <class T>
class CommandHandler
{
	struct Item
	{
		std::string code;
		std::string description;
		std::function<void(T&, int, std::list<std::string>&)> Callback;
	};
public:
	CommandHandler(T& owner)
		:
		owner(owner)
	{}

	// int = id of person etc. list = args
	void RegisterCommand(const std::string& code, std::function<void(T&, int, std::list<std::string>&)> Callback,const std::string& description)
	{
		size_t key = code.length();
		if (key == 0)
			return;

		auto e = hashmap.find(key);
		if (e == hashmap.end())
		{
			// create vector
			hashmap.insert(std::pair< size_t, std::vector<Item>>(key, std::vector<Item>()));
			e = hashmap.find(key);
		}
		assert(e != hashmap.end());
		
		Item i;
		i.code = code;
		i.Callback = Callback;
		i.description = description;

		e->second.push_back(i);
	}
	bool InterpretCommand(int id, std::list<std::string>& args)
	{
		if (args.size() == 0)
			return false;

		size_t key = args.front().length();

		auto e = hashmap.find(key);
		if (e != hashmap.end())
		{
			// check for correct string
			for (const auto& i : e->second)
			{
				if (i.code.compare(args.front()) == 0)
				{
					// string match!
					args.pop_front();

					i.Callback(owner, id, args);
					return true;
				}
			}
		}


		return false;
	}
	bool InpterpretCString(int id, const char* line)
	{
		std::list< std::string > args;
		std::string curStr;
		while (*line != '\0')
		{
			if (*line == ' ')
			{
				if (curStr.length())
				{
					args.push_back(std::move(curStr));
					curStr.clear();
				}
			}
			else
			{
				// valid char
				curStr.push_back(*line);
			}
			line++;
		}
		if (curStr.length())
			args.push_back(std::move(curStr));

		return InterpretCommand(id, args);
	}
	void GenerateHelpMessage()
	{
		helpmsg.clear();

		helpmsg += std::string("commands:\n");

		for (const auto& v : hashmap)
		{
			for (const auto& i : v.second)
			{
				helpmsg += std::string(i.code + std::string(" - ") + i.description + std::string("\n"));
			}
		}
	}
	const std::string& GetHelpMessage() const
	{
		return helpmsg;
	}
private:
	std::map < size_t, std::vector<Item>> hashmap;
	T& owner;

	std::string helpmsg;
};