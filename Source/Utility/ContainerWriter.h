#pragma once
#include "DataContainer.h"
#include "Writer.h"

class ContainerWriter final : public Writer
{
public:
	ContainerWriter(DataContainer& r)
		:
		ref(r)
	{}
	virtual ~ContainerWriter(){}
protected:
	virtual void _write(const void* pSrc, size_t len) override final
	{
		ref.append(pSrc, len);
	}
private:
	DataContainer& ref;
};
