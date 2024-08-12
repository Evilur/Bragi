#ifndef BRAGI_FLAC_TO_OPUS_H
#define BRAGI_FLAC_TO_OPUS_H

#include "audio_to_opus.h"

#include <FLAC++/decoder.h>

class FlacToOpus final : public AudioToOpus, private FLAC::Decoder::Stream {
public:
	FlacToOpus();

	int Convert(char* in, unsigned char* out) override;

	FLAC__StreamDecoderReadStatus read_callback(FLAC__byte* buffer, size_t* bytes) override;

	FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame* frame, const FLAC__int32* const* buffer) override;

	void error_callback(FLAC__StreamDecoderErrorStatus status) override;
};

#endif