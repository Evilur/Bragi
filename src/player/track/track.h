#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include "converter/audio_to_opus.h"

#include <dpp/dpp.h>
#include <string>

class Track {
public:
	virtual ~Track();

	const std::string &GetTitle() const;

	const unsigned short &GetDuration() const;

	virtual dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const = 0;

	virtual std::string GetTrackData() const = 0;

	virtual int GetOpus(unsigned char* out) = 0;

	virtual bool CanRead() = 0;

protected:
	AudioToOpus* _converter;

	/* Important members */
	const std::string _title;
	const unsigned short _duration;

	Track(const std::string &title, const unsigned short &duration);
};

#endif