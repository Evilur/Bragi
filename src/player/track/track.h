#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include <dpp/dpp.h>
#include <string>
#include "master.h"

class Track {
public:
	Track(const unsigned short &duration);

	virtual ~Track();

	const unsigned short &GetDuration() const;

	virtual dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const = 0;

	virtual std::string GetTrackData() const = 0;

	void Abort();

	void AsyncPlay(const dpp::voiceconn* voiceconn, const byte speed_percent);

	virtual Track* Next() = 0;

protected:
	const unsigned short _duration;

	virtual void Play(const dpp::voiceconn* voiceconn) = 0;

	const byte &GetSpeed() const;

	bool IsAborted();

private:
	std::thread* _play_thread = nullptr;
	bool _is_aborted = false;
	byte _speed_percent = 100;
};

#endif