#pragma once
#include <stdexcept>
#include <string>

// template for reader of any kind (file, stream)
class Reader
{
public:

	// should throw out_of_range
	template <class T>
	T read()
	{
		T dst;
		_read(&dst,sizeof(T));
		return dst;
	}

	template <class T>
	void readStruct(T* dst)
	{
		_read(dst, sizeof(T));
	}
	// template variables
	int8_t readChar()
	{
		return read<int8_t>();
	}
	uint8_t readByte()
	{
		return read<uint8_t>();
	}
	bool readBool() 
	{
		return readChar() != 0;
	}
	int16_t readShort()
	{
		return read<int16_t>();
	}
	int32_t readInt()
	{
		return read<int32_t>();
	}
	int64_t readInt64()
	{
		return read<int64_t>();
	}
	float readFloat()
	{
		return read<float>();
	}
	double readDouble()
	{
		return read<double>();
	}
	std::string readString()
	{
		std::string res;
		char c;
		while (c = readChar())
		{
			res.push_back(c);
		}
		return res;
	}
protected:
	virtual void _read(void* dstBuffer, size_t len) = 0;
};