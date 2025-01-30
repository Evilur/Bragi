#ifndef BRAGI_DEEZER_TRACK_H
#define BRAGI_DEEZER_TRACK_H

#include <openssl/blowfish.h>
#include "track.h"
#include "util/dictionary.h"
#include "web/http_client.h"
#include "master.h"
#include "sender/opus_sender.h"

class DeezerTrack final : public Track {
public:
	DeezerTrack(const unsigned short track_duration,
	            const unsigned int track_id, const std::string &track_title, const std::string &track_token,
	            const unsigned int album_id, const std::string &album_title, const std::string &album_picture_id,
	            const unsigned int artist_id, const std::string &artist_name, const std::string &artist_picture_id,
	            const unsigned short search_total, const unsigned short search_next, const std::string &search_query);

	~DeezerTrack();

	dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const override;

	std::string GetTrackData() const override;

	Track* Next() const override;

private:
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
	std::string _encrypted_data_url;
	std::thread* _init_thread = nullptr;
	HttpClient* _http = nullptr;

	void Play(const dpp::voiceconn* voiceconn, const byte speed_percent) override;

	void GetKey(unsigned char* buffer);
};

#endif