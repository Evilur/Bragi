#ifndef BRAGI_DEEZER_TRACK_H
#define BRAGI_DEEZER_TRACK_H

#include "track.h"
#include "util/dictionary.h"

class DeezerTrack final : public Track {
public:
	DeezerTrack(const std::string &id, const std::string &album_id, const std::string &artist_id,
	            const std::string &title, const std::string &album_title, const std::string &artist_name,
	            const std::string &album_picture, const std::string &artist_picture,
	            const std::string &duration, const std::string &url,
	            const unsigned short &total, const unsigned short &next);

	int GetOpus(unsigned char* out) override;

	bool CanRead() override;

	dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const override;

	std::string GetTrackData() const override;

private:
	/* Ids */
	const unsigned int _id;
	const unsigned int _album_id;
	const unsigned int _artist_id;

	/* Titles */
	const std::string _album_title;
	const std::string _artist_name;

	/* Pictures */
	const std::string _album_picture;
	const std::string _artist_picture;

	/* Others */
	const std::string _url;
	const unsigned short _total;
	const unsigned short _next;
};

#endif