#include "deezer_track.h"
#include "util/parser.h"
#include "util/color.h"
#include "util/logger.h"
#include "client/deezer_client.h"

DeezerTrack::DeezerTrack(const std::string &id, const std::string &album_id, const std::string &artist_id,
                         const std::string &title, const std::string &album_title, const std::string &artist_name,
                         const std::string &album_picture, const std::string &artist_picture,
                         const std::string &duration, const std::string &token,
                         const unsigned short &total, const unsigned short &next) :
		Track(Parser::ToUInt16(duration.c_str())),
		_id(Parser::ToUInt32(id.c_str())), _album_id(Parser::ToUInt32(album_id.c_str())), _artist_id(Parser::ToUInt32(artist_id.c_str())),
		_title(title), _album_title(album_title), _artist_name(artist_name),
		_album_picture("https://e-cdns-images.dzcdn.net/images/cover/" + album_picture + "/1000x1000-000000-80-0-0.jpg"),
		_artist_picture("https://e-cdns-images.dzcdn.net/images/artist/" + artist_picture + "/1000x1000-000000-80-0-0.jpg"),
		_token(token), _total(total), _next(next) { }

int DeezerTrack::GetOpus(unsigned char* out) {
	throw std::logic_error("Not implemented yet");
}

bool DeezerTrack::CanRead() {
	throw std::logic_error("Not implemented yet");
}

dpp::message DeezerTrack::GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const {
	std::string msg_body = '\n' + std::format(DIC_TRACK_DURATION, Parser::Time(_duration));
	if (is_playing_now) msg_body.insert(0, std::format(DIC_TRACK_PLAYING_NOW, _title));
	else msg_body.insert(0, std::format(DIC_TRACK_ADD_TO_PLAYLIST, _title));

	dpp::message result = dpp::message(channel_id, msg_body);
	result.add_embed(dpp::embed()
			                 .set_color(Color::RED)
			                 .add_field(DIC_TRACK_ALBUM, _album_title)
			                 .add_field(DIC_TRACK_ARTIST, _artist_name)
			                 .set_thumbnail(_artist_picture)
			                 .set_image(_album_picture));
	return result;
}

std::string DeezerTrack::GetTrackData() const { return std::format(DIC_SLASH_LIST_FULL_TRACK_DATA, _title, _album_title, _artist_name); }

void DeezerTrack::OnInit() {
	_url = DeezerClient::GetEncodedTrackUrl(_token);
}