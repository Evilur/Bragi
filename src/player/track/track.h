#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include "converter/audio_to_opus.h"

#include <dpp/dpp.h>
#include <string>

class Track {
public:
	virtual ~Track();

	virtual dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) = 0;

	virtual int GetOpus(unsigned char* out) = 0;

	virtual bool CanRead() = 0;

protected:
	AudioToOpus* _converter;

	virtual std::string GetMessageBody(const bool &is_playing_now) = 0;

	static std::string GetFormattedDuration(const unsigned short &duration);
};

#endif