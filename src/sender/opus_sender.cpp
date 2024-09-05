#include "audio_to_opus.h"

OpusSender::OpusSender(dpp::voiceconn* voiceconn) {
	_encoder = opus_encoder_create(FREQ, CHANNELS, OPUS_APPLICATION_AUDIO, nullptr);
	_voiceconn = voiceconn;
}

OpusSender::~OpusSender() {
	opus_encoder_destroy(_encoder);
	_encoder = nullptr;
}