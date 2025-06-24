#ifndef BRAGI_DEEZER_TRACK_H
#define BRAGI_DEEZER_TRACK_H

#include "track.h"
#include "locale/locale.h"
#include "web/http_client.h"
#include "master.h"
#include "sender/opus_sender.h"

#include <openssl/blowfish.h>

class DeezerTrack final : public Track {
public:
	enum Quality { MP3_128, MP3_320, FLAC };

	DeezerTrack(const unsigned short track_duration, Quality quality,
	            const unsigned int track_id, const std::string &track_title, const std::string &track_token,
	            const unsigned int album_id, const std::string &album_title, const std::string &album_picture_id,
	            const unsigned int artist_id, const std::string &artist_name, const std::string &artist_picture_id,
	            const unsigned short search_total, const unsigned short search_next, const std::string &search_query);

	~DeezerTrack();

	dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const override;

	std::string GetTrackData() const override;

	Track* Next() const override;

private:
	const Quality _quality;
	std::string _data_url;

	const struct {
		const unsigned int id;
		const std::string title;
		const std::string token;
	} _track;

	const struct {
		const unsigned int id;
		const std::string title;
		const std::string picture_id;
	} _album;

	const struct {
		const unsigned int id;
		const std::string name;
		const std::string picture_id;
	} _artist;

	struct {
		const unsigned short total;
		const unsigned short next;
		const std::string next_query;
	} _search;

	BF_KEY _bf_key;
	std::thread* _init_thread = nullptr;
	HttpClient* _http = nullptr;

	void Play(dpp::discord_voice_client* const voiceclient, const unsigned char speed_percent) override;

	void GetKey(unsigned char* buffer);
};

#endif
