#include "track.h"
#include "util/logger.h"

const unsigned short &Track::GetDuration() const { return _duration; }

Track::Track(const unsigned short &duration) : _duration(duration) { }

Track::~Track() {
	if (_play_thread != nullptr && _play_thread->joinable()) _play_thread->join();
	delete _play_thread;
	_play_thread = nullptr;
}

void Track::Abort() {
	_is_aborted = true;
	if (_play_thread->joinable()) _play_thread->join();
}

void Track::AsyncPlay(const dpp::voiceconn* voiceconn, const byte speed_percent) {
	_speed_percent = speed_percent;
	_play_thread = new std::thread(&Track::Play, this, voiceconn);
}

bool Track::IsAborted() { return _is_aborted; }

const byte &Track::GetSpeed() const { return _speed_percent; }