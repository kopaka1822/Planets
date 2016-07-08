#pragma once
#include <memory>
#include <string>

class WaveFile
{
public:
/*	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};*/
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned int chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned int subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned int sampleRate;
		unsigned int bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned int dataSize;
	};
public:
	WaveFile(const std::string& file);
	~WaveFile();

	const WaveHeaderType& GetHeader() const;
	const std::unique_ptr< unsigned char[] >& GetData() const;
private:
	std::unique_ptr< unsigned char[] > rawData;
	WaveHeaderType header;
};
