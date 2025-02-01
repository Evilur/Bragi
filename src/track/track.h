#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include <dpp/dpp.h>
#include <string>
#include "master.h"
#include "sender/opus_sender.h"

class Track {
public:
	Track(const unsigned short &duration);

	virtual ~Track();

	const unsigned short &GetDuration() const;

	virtual dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const = 0;

	virtual std::string GetTrackData() const = 0;

	void Abort();

	void AsyncPlay(const dpp::voiceconn* voiceconn, const byte speed_percent);

	virtual Track* Next() const = 0;

protected:
	const unsigned short duration;

	virtual void Play(const dpp::voiceconn* voiceconn, const byte speed_percent) = 0;

	void SetOpusSender(OpusSender* sender);

private:
	std::thread* _play_thread = nullptr;
	OpusSender* _opus_sender = nullptr;
};

#endif