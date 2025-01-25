#include "deezer_track.h"
#include "util/parser.h"
#include "util/color.h"
#include "util/logger.h"
#include "client/deezer_client.h"
#include "sender/flac_sender.h"
#include "master.h"

#include <openssl/md5.h>
#include <openssl/blowfish.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

DeezerTrack::DeezerTrack(const std::string &id, const std::string &album_id, const std::string &artist_id,
                         const std::string &title, const std::string &album_title, const std::string &artist_name,
                         const std::string &album_picture, const std::string &artist_picture,
                         const std::string &duration, const std::string &token, const std::string &next_query,
                         const unsigned short &total, const unsigned short &next) :
		Track(Parser::ToUInt16(duration.c_str())),
		_track(Parser::ToUInt32(id.c_str()), title, token),
		_album(Parser::ToUInt32(album_id.c_str()), album_title, album_picture),
		_artist(Parser::ToUInt32(artist_id.c_str()), artist_name, artist_picture),
		_search(total, next, next_query) {
	_init_thread = new std::thread([this]() {
		/* Set the url of the encrypted track data */
		_encrypted_data_url = DeezerClient::GetEncodedTrackUrl(_track.token);

		/* Set the blowfish cipher key */
		unsigned char key_buffer[MD5_DIGEST_LENGTH];
		GetKey(key_buffer);
		BF_set_key(&_bf_key, MD5_DIGEST_LENGTH, key_buffer);
	});
}

DeezerTrack::~DeezerTrack() {
	if (_init_thread->joinable()) _init_thread->join();
	delete _init_thread;
	_init_thread = nullptr;

	delete _http;
	_http = nullptr;
}

dpp::message DeezerTrack::GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const {
	std::string msg_body = '\n' + std::format(DIC_TRACK_DURATION, Parser::Time(duration));
	if (is_playing_now) msg_body.insert(0, std::format(DIC_TRACK_PLAYING_NOW, _track.title));
	else msg_body.insert(0, std::format(DIC_TRACK_ADD_TO_PLAYLIST, _track.title));

	dpp::message result = dpp::message(channel_id, msg_body);
	result.add_embed(dpp::embed()
			                 .set_color(Color::RED)
			                 .add_field(DIC_TRACK_ALBUM, _album.title)
			                 .add_field(DIC_TRACK_ARTIST, _artist.name)
			                 .set_image("https://e-cdns-images.dzcdn.net/images/cover/" + _album.picture_id + "/1000x1000-000000-80-0-0.jpg")
			                 .set_thumbnail("https://e-cdns-images.dzcdn.net/images/artist/" + _artist.picture_id + "/1000x1000-000000-80-0-0.jpg"));
	return result;
}

std::string DeezerTrack::GetTrackData() const { return std::format(DIC_SLASH_LIST_FULL_TRACK_DATA, _track.title, _artist.name); }

Track* DeezerTrack::Next() const {
	if (_search.next >= _search.total) return nullptr;  //TODO: Check this
	return DeezerClient::Search(_search.next_query, _search.next);
}

void DeezerTrack::Play(const dpp::voiceconn* voiceconn, const byte speed_percent) {
	/* Wait for initialization */
	if (_init_thread->joinable()) _init_thread->join();

	/* Create a new http client */
	_http = new HttpClient(_encrypted_data_url);

	/* Lambda for reading the track data by buffers */
	auto read_buffer = [this](byte* buffer, unsigned long* buffer_size) {
		/* Set the chunk size */
		constexpr int chunk_size = 2048;

		/* If http steam has ended, or we have aborted the playback */
		if (!_http->CanRead()) {
			*buffer_size = 0;
			return false;
		}

		/* Read 3 raw chunks */
		_http->Read((char*)buffer, chunk_size * 3);

		/* Set the buffer size according to the recieved data size */
		*buffer_size = _http->PrevCount();

		/* Set the init vectors */
		unsigned char ivec[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7 };

		/* Decrypt the first chunk */
		if (*buffer_size >= chunk_size)
			BF_cbc_encrypt(buffer, buffer, chunk_size, &_bf_key, ivec, BF_DECRYPT);
		return true;
	};

	/* Set the opus sender */
	SetSender(new FlacSender(voiceconn, speed_percent, &read_buffer));
}

void DeezerTrack::GetKey(unsigned char* buffer) {
	/* The salt for getting the key for track decrypting */
	constexpr unsigned char salt[] = "g4el58wc0zvf9na1";

	/* Get the md5 hash sum of the track id */
	const std::string id_str = std::to_string(_track.id);
	unsigned char md5_digest[MD5_DIGEST_LENGTH];
	MD5((unsigned char*)id_str.c_str(), id_str.size(), md5_digest);
	unsigned char md5_sum[MD5_DIGEST_LENGTH * 2];
	constexpr unsigned char alph[] = "0123456789abcdef";
	for (char i = 0; i < MD5_DIGEST_LENGTH; i++) {
		const char cur_i = i * 2;
		md5_sum[cur_i] = alph[md5_digest[i] >> 4];
		md5_sum[cur_i + 1] = alph[md5_digest[i] & 0xF];
	}

	/* Build the key */
	const unsigned char* md5_sum_fst_half = md5_sum;  //Get the first half of the hash
	const unsigned char* md5_sum_sec_half = md5_sum + MD5_DIGEST_LENGTH;  //Get the second half of the hash
	for (char i = 0; i < MD5_DIGEST_LENGTH; i++) buffer[i] = salt[i] ^ md5_sum_fst_half[i] ^ md5_sum_sec_half[i];
}

#pragma GCC diagnostic pop