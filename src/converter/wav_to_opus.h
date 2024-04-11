#ifndef BRAGI_WAV_TO_OPUS_H
#define BRAGI_WAV_TO_OPUS_H

#include "opus_converter.h"

class WavToOpus : public OpusConverter {
public:
	WavToOpus();
	~WavToOpus() override;
	
	int Convert(char *in, unsigned char *out) override;
};

#endif
