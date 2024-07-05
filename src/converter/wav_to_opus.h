#ifndef BRAGI_WAV_TO_OPUS_H
#define BRAGI_WAV_TO_OPUS_H

#include "audio_to_opus.h"

class WavToOpus final : public AudioToOpus {
public:
	WavToOpus();

	int Convert(char* in, unsigned char* out) override;
};

#endif