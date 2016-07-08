#pragma once
#include <exception>
#include <string>
#include "Exception.h"

// a non critical error for the game, the game should not shut down forcefully if this is thrown
class GameError : public Exception
{
public:
	GameError(const std::string& msg)
		:
		Exception(msg)
	{}
	/*GameError(const std::string& msg, unsigned int code)
		:
		errmes(msg)
	{
		errmes += " code: ";
		char buffer[50];
		sprintf_s(buffer, "%x", code);
		errmes += buffer;
	}*/
	virtual ~GameError()
	{}
};
