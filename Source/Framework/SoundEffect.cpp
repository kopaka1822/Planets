#include "SoundEffect.h"
#include "../Utility/FileException.h"
#include "Sound.h"
#include "../System/Log.h"

SoundEffect::SoundEffect(const std::string& filename, float vol)
	:
	wave(filename),
	curVol(vol)
{
	// Check Wave File
	const WaveFile::WaveHeaderType& waveFileHeader = wave.GetHeader();
	
	// Check that the chunk ID is the RIFF format.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
		throw ExInvalidFile(filename, "chunkId != RIFF");

	// Check that the file format is the WAVE format.
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
		throw ExInvalidFile(filename, "format != WAVE");

	// Check that the sub chunk ID is the fmt format.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
		throw ExInvalidFile(filename, "subChunkId != ftm");

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (waveFileHeader.audioFormat != 1) // 1 = WAVE_FORMAT_PCM
		throw ExInvalidFile(filename, "audioFormat != WAVE_FORMAT_PCM");

	// Check that the wave file was recorded in stereo format.
	if (waveFileHeader.numChannels != 2)
		throw ExInvalidFile(filename, "only stereo supported");

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if (waveFileHeader.sampleRate != Sound::SAMPLERATE)
		throw ExInvalidFile(filename, "sample rate not supported (only 44100 allowed)");

	// Ensure that the wave file was recorded in 16 bit format.
	if (waveFileHeader.bitsPerSample != Sound::BITRATE)
		throw ExInvalidFile(filename, "only 16 bits per sample are allowed");

	// Check for the data chunk header.
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
		throw ExInvalidFile(filename, "ataChunkId!=data");
}
SoundEffect::~SoundEffect()
{
	Stop();
}
int SoundEffect::CollectGarbage(int channel)
{
	if (bStopping)
		return 0; // this files we be deleted anyway
	// delete sounds that are not playing anymore
	

	LockGuard g(muSounds);

	for (auto i = pSounds.begin(), end = pSounds.end(); i != end; i++)
	{
		if ((*i)->GetChannel() == channel)
		{
			pSounds.erase(i);
			return 1;
		}
	}
	
	return 0;
}
void SoundEffect::Play(int channel)
{
	if (channel == -1)
		return; // no free channels
	// create new sound
	std::unique_ptr< MusicFile > pMusic = std::unique_ptr< MusicFile >(new MusicFile(wave, curVol));
	if (pMusic)
	{
		pMusic->Play(channel, false);

		LockGuard g(muSounds);
		pSounds.push_back(std::move(pMusic));
		g.Unlock();
	}
}
void SoundEffect::Stop()
{
	bStopping = true;

	LockGuard g(muSounds);
	pSounds.clear();
	g.Unlock();
}
void SoundEffect::SetVolume(float vol)
{
	curVol = vol;
}