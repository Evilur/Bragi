#ifndef BRAGI_OPUS_CONVERTER_H
#define BRAGI_OPUS_CONVERTER_H

#include "opus/opus.h"

class OpusConverter {
public:
	static const int FREQ = 48000;
	static const int FRAME_SIZE = 960;
	static const int CHANNELS = 2;
	static const int PCM_CHUNK_SIZE = FRAME_SIZE * CHANNELS * sizeof(opus_int16);
	static const int OPUS_CHUNK_SIZE = 1024;
	
	virtual ~OpusConverter() = default;
	virtual int Convert(char* in, unsigned char* out) = 0;
	
protected:
	OpusEncoder* encoder {};
};

#endif
