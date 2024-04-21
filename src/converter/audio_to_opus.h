#ifndef BRAGI_AUDIO_TO_OPUS_H
#define BRAGI_AUDIO_TO_OPUS_H

#include "opus/opus.h"

class AudioToOpus {
public:
	static const int FREQ = 48000;
	static const int FRAME_SIZE = 960;
	static const int CHANNELS = 2;
	static const int PCM_CHUNK_SIZE = FRAME_SIZE * CHANNELS * sizeof(opus_int16);
	static const int OPUS_CHUNK_SIZE = 1024;
	
	virtual ~AudioToOpus() = default;
	virtual int Convert(char* in, unsigned char* out) = 0;
	
protected:
	OpusEncoder* _encoder{};
};

#endif
