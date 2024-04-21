#ifndef BRAGI_AUDIO_TO_OPUS_H
#define BRAGI_AUDIO_TO_OPUS_H

#include "opus/opus.h"

class AudioToOpus {
public:
	static constexpr int FREQ = 48000;
	static constexpr int FRAME_SIZE = 960;
	static constexpr int CHANNELS = 2;
	static constexpr int PCM_CHUNK_SIZE = FRAME_SIZE * CHANNELS * sizeof(opus_int16);
	static constexpr int OPUS_CHUNK_SIZE = 1024;
	
	AudioToOpus();
	virtual ~AudioToOpus();
	
	virtual int Convert(char* in, unsigned char* out) = 0;
protected:
	OpusEncoder* _encoder;
};

#endif
