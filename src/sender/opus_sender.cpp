#include "opus_sender.h"

OpusSender::OpusSender(const dpp::voiceconn* voiceconn) : _voiceconn(voiceconn) {
	_encoder = opus_encoder_create(FREQ, CHANNELS, OPUS_APPLICATION_AUDIO, nullptr);
}

OpusSender::~OpusSender() {
	opus_encoder_destroy(_encoder);
	_encoder = nullptr;
}