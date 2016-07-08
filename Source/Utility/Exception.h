#pragma once
#include <exception>
#include <string>

class Exception : public std::exception
{
public:
    Exception(const char* txt)
    {
        emes = txt;
    }
    Exception(const std::string& s)
    {
        emes = s;
    }
#ifdef _WIN32
	virtual const char* what() const override
#else
	virtual const char* what() const noexcept(true) override
#endif
	{
		return emes.c_str();
	}
protected:
    std::string emes;
};
