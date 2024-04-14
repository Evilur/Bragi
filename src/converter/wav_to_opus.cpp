#include "wav_to_opus.h"
#include "util/logger.h"

WavToOpus::WavToOpus() {
	_encoder = opus_encoder_create(FREQ, CHANNELS, OPUS_APPLICATION_AUDIO, nullptr);
}

int WavToOpus::Convert(char *in, unsigned char *out) {
	return opus_encode(_encoder, (opus_int16*)in, FRAME_SIZE, out, OPUS_CHUNK_SIZE);
}

WavToOpus::~WavToOpus() {
	opus_encoder_destroy(_encoder);
}
