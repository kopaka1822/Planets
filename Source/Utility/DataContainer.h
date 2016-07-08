#pragma once
#include <string.h>
#include <exception>
#include <stdlib.h> //malloc (should be faster and has realloc)
#include <utility>
#include <assert.h>
#include <algorithm>

class DataContainer
{
public:
	DataContainer()
	{
		//startAlloc();
	}
	DataContainer(unsigned int allocSize)
	{
		Realloc(allocSize);
	}
	DataContainer(const DataContainer& copy) = delete;
	DataContainer(DataContainer&& move)
	{
		Swap(*this, move);
	}
	inline static void Swap(DataContainer& s1, DataContainer& s2)
	{
		std::swap(s1.data, s2.data);
		std::swap(s1.lenAlloc, s2.lenAlloc);
		std::swap(s1.lenUsed, s2.lenUsed);
	}
	DataContainer(const void* cdata, unsigned int dataLen)
	{
		set(cdata, dataLen);
	}
	~DataContainer()
	{
		Dispose();
	}
	DataContainer& operator=(const DataContainer& rhs) = delete;
	DataContainer& operator=(DataContainer&& rhs)
	{
		Swap(*this, rhs);
		return *this;
	}
	DataContainer duplicate() const
	{
		DataContainer res(data, lenUsed);
		return res;
	}
	template <typename T>
	void operator+=(const T& rhs)
	{
		unsigned int size = sizeof(T);
		if (size + lenUsed >= lenAlloc)
			Realloc(size + lenUsed);
		memcpy(((char*)data) + lenUsed, &rhs, size);

		lenUsed += size;
	}
	void operator-=(size_t size)
	{
		assert(size <= lenUsed);
		lenUsed -= size;
	}

	const void* operator[](size_t index) const
	{
		assert(index < lenUsed);
		return (void*)((char*)data + index);
	}
	void* operator[](size_t index)
	{
		assert(index < lenUsed);
		return (void*)((char*)data + index);
	}
	size_t length() const
	{
		return lenUsed;
	}
	void clear()
	{
		lenUsed = 0;
	}
	const void* start() const
	{
		return this->operator[](0);
	}
	void set(const void* cdata, size_t dataLen)
	{
		Dispose();
		lenAlloc = (dataLen / allocIntervall + 1) * allocIntervall;
		lenUsed = dataLen;
		data = malloc(lenAlloc);
		if (data == nullptr)
			throw std::bad_alloc();
		memcpy(data, cdata, dataLen);
	}
	void append(const void* cdata, size_t dataLen)
	{
		if (lenUsed + dataLen >= lenAlloc)
			Realloc(lenUsed + dataLen);

		memcpy((char*)data + lenUsed, cdata, dataLen);
		lenUsed += dataLen;
	}
	void skip(size_t len)
	{
		if (lenUsed + len >= lenAlloc)
			Realloc(lenUsed + len);

		lenUsed += len;
	}
	void reserve(size_t len)
	{
		if (lenAlloc < len)
		{
			Realloc(len);
		}
	}
private:
	void* data = nullptr;
	size_t lenAlloc = 0;
	size_t lenUsed = 0;
	static const size_t allocIntervall = 4096;

private:
	void startAlloc()
	{
		data = malloc(allocIntervall);
		if (!data)
			throw std::bad_alloc();
		lenAlloc = allocIntervall;
		lenUsed = 0;
	}
	void Realloc(size_t newSize)
	{
		if (newSize > lenAlloc)
		{
			lenAlloc = std::max((newSize / allocIntervall + 1) * allocIntervall, 2 * lenAlloc);
			data = realloc(data, lenAlloc);
			if (data == nullptr)
				throw std::bad_alloc();
		}
	}
	void Dispose()
	{
		if (data != nullptr)
		{
			free(data);
			data = nullptr;
			lenAlloc = 0;
			lenUsed = 0;
		}
	}
};

class BufferWriter
{
public:
	BufferWriter(void* buffer)
		:
		data((char*) buffer)
	{}
	template <class T>
	void write(const T& itm)
	{
		memcpy(data + wPos, &itm, sizeof(T));
		wPos += sizeof(T);
	}
	void writeByte(char item)
	{
		write<char>(item);
	}
	void writeChar(char item)
	{
		write<char>(item);
	}
	void writeShort(short item)
	{
		write<short>(item);
	}
	void writeInt(int item)
	{
		write<int>(item);
	}
	void writeFloat(float item)
	{
		write<float>(item);
	}
	void writeDouble(double item)
	{
		write<double>(item);
	}
	void writeString(const char* text)
	{
		unsigned int i = 0;
		while (text[i] != 0)
		{
			write<char>(text[i]);
			++i;
		}
		write<char>(0);
	}
	void writeArray(const void* rdata, unsigned int len)
	{
		memcpy(data + wPos, rdata, len);
		wPos += len;
	}
	template <class T>
	void writeStruct(const T& str)
	{
		write<T>(str);
	}

	template <class T>
	void writeStructArray(const T* data, unsigned int n)
	{
		writeArray((const void*)data, sizeof(T) * n);
	}
	unsigned int GetLength() const
	{
		return wPos;
	}
private:
	char* data;
	unsigned int wPos = 0;
};

class ContainerWriterr
{
public:
	ContainerWriterr(DataContainer& ref)
		:
		ref(ref)
	{}
	void writeByte(char item)
	{
		ref += item;
	}
	void writeChar(char item)
	{
		ref += item;
	}
	void writeShort(short item)
	{
		ref += item;
	}
	void writeInt(int item)
	{
		ref += item;
	}
	void writeLongLong(long long item)
	{
		ref += item;
	}
	void writeFloat(float item)
	{
		ref += item;
	}
	void writeDouble(double item)
	{
		ref += item;
	}
	void writeString(const char* text)
	{
		unsigned int i = 0;
		while (text[i] != 0)
		{
			ref += text[i];
			i++;
		}
		ref += char(0);
	}
	void writeStringNoNull(const char* text)
	{
		unsigned int i = 0;
		while (text[i] != 0)
		{
			ref += text[i];
			i++;
		}
	}
	void writeArray(const void* data, unsigned int len)
	{
		ref.append(data,len);
	}
	template <class T>
	void writeStruct(const T& str)
	{
		ref += str;
	}

	template <class T>
	void writeStructArray(const T* data, unsigned int n)
	{
		ref.append(data, n * sizeof(T));
	}
	void rewind(unsigned int size)
	{
		ref -= size;
	}
private:
	DataContainer& ref;
};

class BufferReader
{
public:
	BufferReader(const char* buffer)
		:
		data(buffer)
	{

	}
	void Skip(unsigned int nBytes)
	{
		rPos += nBytes;
	}
	unsigned char readByte()
	{
		rPos += sizeof(char);
		return (unsigned char)data[rPos - sizeof(char)];
	}
	char readChar()
	{
		rPos += sizeof(char);
		return data[rPos - sizeof(char)];
	}
	short readShort()
	{
		rPos += sizeof(short);
		return *(short*)(data + rPos - sizeof(short));
	}
	int readInt()
	{
		rPos += sizeof(int);
		return *(int*)(data + rPos - sizeof(int));
	}
	float readFloat()
	{
		rPos += sizeof(float);
		return *(float*)(data + rPos - sizeof(float));
	}
	double readDouble()
	{
		rPos += sizeof(double);
		return *(double*)(data + rPos - sizeof(double));
	}
	const char* readString()
	{
		unsigned int len = strlen(reinterpret_cast<char*>(data[rPos])) + 1;
		rPos += len;
		return data + rPos - len;
	}
	const void* readArray(unsigned int size)
	{
		rPos += size;
		return &data[rPos - size];
	}

	template <class T>
	T readStruct()
	{
		rPos += sizeof(T);
		return *(T*)(data + rPos - sizeof(T));
	}

	template <class T>
	const T* readStructArray(unsigned int n)
	{
		rPos += n * sizeof(T);
		return (T*)(data + rPos - n * sizeof(T));
	}
private:
	const char* data;
	unsigned int rPos = 0;
};

#include "ContainerReader.h"
#include "ContainerWriter.h"