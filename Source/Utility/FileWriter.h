#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "Writer.h"
#include <stdio.h>

class FileWriter : public Writer
{
public:
	FileWriter(const std::string& filename)
	{
		pFile = fopen(filename.c_str(), "wb");
	}
	bool isOpen() const
	{
		return pFile != nullptr;
	}
protected:
	virtual void _write(const void* pSrc, size_t len) override
	{
		fwrite(pSrc, len, 1, pFile);
	}
private:
	FILE* pFile = nullptr;
};