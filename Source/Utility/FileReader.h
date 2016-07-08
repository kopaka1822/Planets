#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Reader.h"
#include "Tools.h"
#include <stdio.h>
#include <stdexcept>

class FileReader : public Reader
{
public:
	FileReader(const std::string& filename)
	{
		pFile = fopen(filename.c_str(), "rb");
	}
	bool isOpen() const
	{
		return pFile != nullptr;
	}
protected:
	virtual void _read(void* dstBuffer, size_t len) override
	{
		size_t count = fread(dstBuffer, len, 1, pFile);
		if(count != 1)
			throw std::out_of_range(tool::getClassname(*this));
	}
private:
	FILE* pFile = nullptr;
};