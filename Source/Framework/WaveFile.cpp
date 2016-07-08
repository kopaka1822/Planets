#include "WaveFile.h"
#include <exception>
#include <stdio.h>
#include "../Utility/FileException.h"

WaveFile::WaveFile(const std::string& file)
{
	FILE* pFile = fopen(file.c_str(), "rb");

	if (!pFile)
		throw ExMissingFile(file);

	unsigned int count = fread(&header, sizeof(WaveHeaderType), 1, pFile);
	if (count != 1)
		throw ExInvalidFile(file, "invalid header");

	size_t dataSize = header.dataSize;
	rawData = std::unique_ptr< unsigned char[] >(new unsigned char[dataSize]);

	count = fread(rawData.get(), 1, dataSize, pFile);
	if (count != dataSize)
		throw ExInvalidFile(file, "file to short or damaged");

	fclose(pFile);
}
WaveFile::~WaveFile()
{

}
const WaveFile::WaveHeaderType& WaveFile::GetHeader() const
{
	return header;
}
const std::unique_ptr< unsigned char[] >& WaveFile::GetData() const
{
	return rawData;
}
