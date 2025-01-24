#include "track.h"
#include "util/logger.h"

const unsigned short &Track::GetDuration() const { return duration; }

Track::Track(const unsigned short &duration) : duration(duration) { }

Track::~Track() {
	if (_play_thread != nullptr && _play_thread->joinable()) _play_thread->join();
	delete _play_thread;
	_play_thread = nullptr;

	delete _sender;
	_sender = nullptr;
}

void Track::Abort() {
	_sender->Abort();
	if (_play_thread != nullptr && _play_thread->joinable()) _play_thread->join();
}

void Track::AsyncPlay(const dpp::voiceconn* voiceconn, const byte speed_percent) {
	_play_thread = new std::thread(&Track::Play, this, voiceconn, speed_percent);
}

void Track::SetSender(OpusSender* sender) {
	_sender = sender;
	_sender->Run();
}