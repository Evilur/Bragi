#include "track.h"
#include "sender/opus_sender.h"

Track::~Track() {
	delete _sender;
	_sender = nullptr;
}

const unsigned short &Track::GetDuration() const { return _duration; }

Track::Track(const unsigned short &duration) : _duration(duration) { }

void Track::SetSender(OpusSender* converter) { _sender = converter; }

void Track::Send(const char* in, const int size) { _sender->Send(in, size); }