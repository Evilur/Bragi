#include "opus_sender.h"

OpusSender::OpusSender(const dpp::voiceconn* const voiceconn, Track* const track) : _voiceconn(voiceconn), _track(track) {
	_encoder = opus_encoder_create(FREQ, CHANNELS, OPUS_APPLICATION_AUDIO, nullptr);
}

OpusSender::~OpusSender() {
	opus_encoder_destroy(_encoder);
	_encoder = nullptr;
}

bool OpusSender::ReadBuffer(unsigned char* buffer, unsigned long* buffer_size) { return _track->ReadBuffer(buffer, buffer_size); }