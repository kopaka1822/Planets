#pragma once
#include <stdexcept>
#include <string>

class Writer
{
public:

	template <class T>
	void write(const T& t)
	{
		_write(&t, sizeof(T));
	}

	// template variables
	void writeChar(int8_t c)
	{
		write(c);
	}
	void writeByte(uint8_t b)
	{
		write(b);
	}
	void writeBool(bool b)
	{
		writeChar(b);
	}
	void writeShort(int16_t s)
	{
		write(s);
	}
	void writeInt(int32_t i )
	{
		write(i);
	}
	void writeInt64(int64_t l)
	{
		write(l);
	}
	void writeFloat(float f)
	{
		write(f);
	}
	void writeDouble(double d)
	{
		write(d);
	}
	void writeStringNoNull(const char* c)
	{
		while (*c)
		{
			writeChar(*c);
			c++;
		}
	}
	void writeString(const char* c)
	{
		writeStringNoNull(c);
		writeChar('\0');
	}
	void writeString(const std::string& s)
	{
		writeString(s.c_str());
	}
protected:
	virtual void _write(const void* pSrc, size_t len) = 0;
};