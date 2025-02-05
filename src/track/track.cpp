#include "track.h"
#include "util/logger.h"

const unsigned short &Track::GetDuration() const { return duration; }

Track::Track(const unsigned short &duration) : duration(duration) { }

Track::~Track() { /* Abort the opus sender */
	if (_opus_sender != nullptr) _opus_sender->Abort();

	/* Join the play thread */
	if (_play_thread != nullptr && _play_thread->joinable()) _play_thread->join();

	/* Delete the opus sender */
	delete _opus_sender;
	_opus_sender = nullptr;

	/* Delete the play thread */
	delete _play_thread;
	_play_thread = nullptr;
}

void Track::Abort() {
	/* Abort the opus sender */
	if (_opus_sender) _opus_sender->Abort();

	/* Join the play thread */
	if (_play_thread && _play_thread->joinable()) _play_thread->join();

	/* Delete the opus sender */
	delete _opus_sender;
	_opus_sender = nullptr;

	/* Delete the play thread */
	delete _play_thread;
	_play_thread = nullptr;
}

void Track::AsyncPlay(dpp::discord_voice_client* const voiceclient, const byte speed_percent) {
	_play_thread = new std::thread(&Track::Play, this, voiceclient, speed_percent);
}

void Track::SetOpusSender(OpusSender* sender) {
	/* Delete the old opus sender */
	if (_opus_sender) _opus_sender->Abort();
	delete _opus_sender;

	/* Set the new opus sender */
	_opus_sender = sender;
	_opus_sender->Run();
}