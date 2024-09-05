#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include "sender/opus_sender.h"

#include <dpp/dpp.h>
#include <string>

class Track {
public:
	virtual ~Track();

	const unsigned short &GetDuration() const;

	virtual dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const = 0;

	virtual std::string GetTrackData() const = 0;

	virtual void SendOpus(const dpp::voiceconn* voiceconn) = 0;

protected:
	const unsigned short _duration;

	Track(const unsigned short &duration);

	void SetSender(OpusSender* converter);

	void Send(const char* in, const int size);

private:
	OpusSender* _sender;
};

#endif