#include "Console.h"
#include <stdio.h>
#include <iostream>
#include <exception>
#include <stdexcept>

void Console::Write(const char* s)
{
	printf("%s",s);
}
void Console::Write(const std::string& s)
{
	printf("%s", s.c_str());
}
void Console::Write(int i)
{
	printf("%d", i);
}
void Console::Write(float f)
{
	printf("%f", f);
}
void Console::Write(bool b)
{
	if (b)
	{
		printf("true");
	}
	else
	{
		printf("false");
	}
}
void Console::Write(char c)
{
	printf("%c", c);
}
void Console::Write(const std::exception& e)
{
	EndLine();
	Write("[Console] ");
	WriteLine(e.what());
}
void Console::EndLine()
{
	printf("\n");
}

void Console::WriteLine(const char* s)
{
	Write(s);
	EndLine();
}
void Console::WriteLine(const std::string& s)
{
	Write(s);
	EndLine();
}
void Console::WriteLine(int i)
{
	Write(i);
	EndLine();
}
void Console::WriteLine(float f)
{
	Write(f);
	EndLine();
}
void Console::WriteLine(bool b)
{
	Write(b);
	EndLine();
}
void Console::WriteLine(char c)
{
	Write(c);
	EndLine();
}

std::string Console::ReadString()
{
	std::string res;
	std::getline(std::cin, res);
	return res;
}
int Console::ReadInt()
{
	bool bDone = false;
	int res = 0;
	do
	{
		try
		{
			res = std::stoi(ReadString());
			bDone = true;
		}
		catch (std::logic_error& e)
		{
			Write(e);
		}
	} while (!bDone);
	return res;
}
float Console::ReadFloat()
{
	bool bDone = false;
	float res = 0;
	do
	{
		try
		{
			res = std::stof(ReadString());
			bDone = true;
		}
		catch (std::logic_error& e)
		{
			Write(e);
		}
	} while (!bDone);
	return res;
}

void Console::Pause()
{
	Console::ReadString();
}
