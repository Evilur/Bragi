#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include "converter/audio_to_opus.h"

#include <dpp/dpp.h>
#include <string>

class Track {
public:
	virtual ~Track();

	std::string GetTitle();

	std::string GetAlbum();

	std::string GetArtist();

	virtual dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id);

	virtual int GetOpus(unsigned char* out) = 0;

	virtual bool CanRead() = 0;

protected:
	AudioToOpus* _converter;
	std::string _title;
	std::string _album;
	std::string _artist;
	std::string _duration;
};

#endif