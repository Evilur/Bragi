#ifndef BRAGI_FLAC_TO_OPUS_H
#define BRAGI_FLAC_TO_OPUS_H

#include "audio_to_opus.h"

class FlacToOpus final : public AudioToOpus {
public:
	FlacToOpus();

	int Convert(char* in, unsigned char* out) override;
};

#endif