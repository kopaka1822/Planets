#pragma once
#include <string>
#include <exception>

class Console
{
	Console() = delete;
	Console(const Console&) = delete;
public:
	// Output
	static void Write(const char* s);
	static void Write(const std::string& s);
	static void Write(int i);
	static void Write(float f);
	static void Write(bool b);
	static void Write(char c);
	static void Write(const std::exception& e);
	static void EndLine();
	static void WriteLine(const char* s);
	static void WriteLine(const std::string& s);
	static void WriteLine(int i);
	static void WriteLine(float f);
	static void WriteLine(bool b);
	static void WriteLine(char c);

	template<class ArgF>
	static void Write(const ArgF& first)
	{
		Write(first);
	}

	template <class ArgF,class... ArgT>
	static void Write(const ArgF& first,const ArgT&... args)
	{
		Write(first);
		Write(args...);
	}

	// Input
	static std::string ReadString();
	static int ReadInt();
	static float ReadFloat();

	// Other
	static void Pause();
};

