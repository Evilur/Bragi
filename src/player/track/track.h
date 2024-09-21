#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include <dpp/dpp.h>
#include <string>

class Track {
public:
	Track(const unsigned short &duration);

	virtual ~Track();

	const unsigned short &GetDuration() const;

	virtual dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const = 0;

	virtual std::string GetTrackData() const = 0;

	void AsyncPlay(const dpp::voiceconn* voiceconn);

	virtual void Play(const dpp::voiceconn* voiceconn) = 0;

	virtual bool ReadBuffer(unsigned char* buffer, unsigned long* buffer_size) = 0;

protected:
	const unsigned short _duration;
};

#endif