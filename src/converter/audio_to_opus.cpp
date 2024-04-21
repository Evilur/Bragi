#include "audio_to_opus.h"

AudioToOpus::AudioToOpus() { _encoder = opus_encoder_create(FREQ, CHANNELS, OPUS_APPLICATION_AUDIO, nullptr); }

AudioToOpus::~AudioToOpus() {
	opus_encoder_destroy(_encoder);
	_encoder = nullptr;
}
