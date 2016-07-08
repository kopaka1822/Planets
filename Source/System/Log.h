#pragma once
#include <stdio.h>
#include <string>
#include <exception>

class Log
{
public:
	static void NewLog();
	static void Shutdown();
	static void Write(const std::string& txt);
	static void Error(const std::exception& e);
	static void Error(const std::string& s);
	static void Warning(const std::string& w);
	// log without time
	static void Info(const std::string& i);
};

#define _LOGDETAILED

#ifdef _LOGDETAILED
#define LOGDETAIL(txt) Log::Write(txt)
#else
#define LOGDETAIL(txt)
#endif

#ifdef _DEBUG
#define LOGDEBUG(txt) Log::Write(txt)
#else
#define LOGDEBUG(txt)
#endif