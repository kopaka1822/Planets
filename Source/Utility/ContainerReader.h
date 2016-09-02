#pragma once
#include "DataContainer.h"
#include "Reader.h"
#include "Tools.h"

class ContainerReader final : public Reader
{
public:
	ContainerReader(const DataContainer& r)
		:
		ref(r)
	{}
	virtual ~ContainerReader(){}
	void skip(size_t s)
	{
		rPos += s;
	}
	unsigned int length() const
	{
		return ref.length();
	}
	long long remaining() const
	{
		return ref.length() - rPos;
	}
	const char* GetCurrent() const
	{
		return (const char*)ref[rPos];
	}

	// TODO remove this function
	const void* readArray(size_t size)
	{
		if (rPos + size > ref.length())
			throw std::out_of_range(tool::getClassname(*this));

		rPos += size;
		return ref[rPos - size];
	}
protected:
	virtual void _read(void* dstBuffer, size_t len) override final
	{
		if (rPos + len > ref.length())
			throw std::out_of_range(tool::getClassname(*this));

		memcpy(dstBuffer, ref[rPos], len);

		rPos += len;
	}
private:
	const DataContainer& ref;
	size_t rPos = 0;
};
