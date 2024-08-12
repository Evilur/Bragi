#include "audio_to_opus.h"

AudioToOpus::AudioToOpus() { _encoder = opus_encoder_create(FREQ, CHANNELS, OPUS_APPLICATION_AUDIO, nullptr); }

AudioToOpus::~AudioToOpus() {
	opus_encoder_destroy(_encoder);
	_encoder = nullptr;
}

int AudioToOpus::OpusEncode(char* in, unsigned char* out) { return opus_encode(_encoder, (opus_int16*)in, FRAME_SIZE, out, OPUS_CHUNK_SIZE); }