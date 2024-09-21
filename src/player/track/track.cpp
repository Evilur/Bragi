#include "track.h"

const unsigned short &Track::GetDuration() const { return _duration; }

Track::Track(const unsigned short &duration) : _duration(duration) { }

Track::~Track() { }

void Track::AsyncPlay(const dpp::voiceconn* voiceconn) { std::thread(&Track::Play, this, voiceconn).detach(); }