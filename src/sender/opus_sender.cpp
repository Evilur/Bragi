#include "opus_sender.h"
#include "util/logger.h"

OpusSender::OpusSender(const dpp::voiceconn* const voiceconn, const byte speed_percent) : _voiceconn(voiceconn) {
	_encoder = opus_encoder_create(FREQ, CHANNELS, OPUS_APPLICATION_AUDIO, nullptr);
	_resampler = speex_resampler_init(2, 44100, 48000 * 100 / speed_percent, 10, nullptr);
}

OpusSender::~OpusSender() {
	/* Destroy the encoder */
	opus_encoder_destroy(_encoder);
	_encoder = nullptr;

	/* Destroy the resampler */
	speex_resampler_destroy(_resampler);
	_resampler = nullptr;
}