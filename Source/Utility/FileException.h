#pragma once
#include <exception>
#include <string>
#include "Exception.h"

class ExFile : public Exception
{
protected:
	ExFile()
	:
	Exception("")
	{}
};

class ExMissingFile : public ExFile
{
public:
	ExMissingFile(const std::string& file)
	{
		emes = "missing file: " + file;
	}
};

class ExInvalidFile : public ExFile
{
public:
	ExInvalidFile(const std::string& file, const std::string& reason)
	{
		emes = "invalid file: " + file + " | " + reason;
	}
};
