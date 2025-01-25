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
	DeezerTrack(const std::string &id, const std::string &album_id, const std::string &artist_id,
	            const std::string &title, const std::string &album_title, const std::string &artist_name,
	            const std::string &album_picture, const std::string &artist_picture,
	            const std::string &duration, const std::string &token, const std::string &next_query,
	            const unsigned short &total, const unsigned short &next);

	~DeezerTrack();

	dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const override;

	std::string GetTrackData() const override;

	Track* Next() const override;

private:
	/* Ids */
	const unsigned int _id;
	const unsigned int _album_id;
	const unsigned int _artist_id;

	/* Titles */
	const std::string _title;
	const std::string _album_title;
	const std::string _artist_name;

	/* Pictures */
	const std::string _album_picture;
	const std::string _artist_picture;

	/* Others */
	std::string _encrypted_data_url;
	const std::string _token;
	const std::string _next_query;
	const unsigned short _total;
	const unsigned short _next;

	/* Tech data */
	BF_KEY _bf_key;
	std::thread* _init_thread = nullptr;
	HttpClient* _http = nullptr;

	void Play(const dpp::voiceconn* voiceconn, const byte speed_percent) override;

	void GetKey(unsigned char* buffer);
};

#endif