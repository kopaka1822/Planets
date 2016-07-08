#pragma once
#include "DataContainer.h"
#include "Writer.h"

class ContainerWriter : public Writer
{
public:
	ContainerWriter(DataContainer& r)
		:
		ref(r)
	{}
protected:
	virtual void _write(const void* pSrc, size_t len) override
	{
		ref.append(pSrc, len);
	}
private:
	DataContainer& ref;
};