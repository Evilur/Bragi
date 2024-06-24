#include "deezer_track.h"
#include "util/num_parser.h"
#include "util/color.h"
#include "util/logger.h"

int DeezerTrack::GetOpus(unsigned char* out) {
	throw std::logic_error("Not implemented yet");
}

bool DeezerTrack::CanRead() {
	throw std::logic_error("Not implemented yet");
}

DeezerTrack::DeezerTrack(const std::string &id, const std::string &album_id, const std::string &artist_id,
                         const std::string &title, const std::string &album_title, const std::string &artist_name,
                         const std::string &album_picture, const std::string &artist_picture,
                         const std::string &duration, const std::string &url,
                         const unsigned short &total, const unsigned short &next) :
		Track(title, NumParser::ToUInt16(duration.c_str())),
		_id(NumParser::ToUInt32(id.c_str())), _album_id(NumParser::ToUInt32(album_id.c_str())), _artist_id(NumParser::ToUInt32(artist_id.c_str())),
		_album_title(album_title), _artist_name(artist_name),
		_album_picture("https://e-cdns-images.dzcdn.net/images/cover/" + album_picture + "/1000x1000-000000-80-0-0.jpg"),
		_artist_picture("https://e-cdns-images.dzcdn.net/images/artist/" + artist_picture + "/1000x1000-000000-80-0-0.jpg"),
		_url(url), _total(total), _next(next) { }

dpp::message DeezerTrack::GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const {
	std::stringstream msg_body;
	if (is_playing_now) msg_body << std::format(DIC_TRACK_PLAYING_NOW, _title);
	else msg_body << std::format(DIC_TRACK_ADD_TO_PLAYLIST, _title);
	msg_body << '\n' << std::format(DIC_TRACK_DURATION, NumParser::Time(_duration));

	dpp::message result = dpp::message(channel_id, msg_body.str());
	result.add_embed(dpp::embed()
			                 .set_color(Color::RED)
			                 .add_field(DIC_TRACK_ALBUM, _album_title)
			                 .add_field(DIC_TRACK_ARTIST, _artist_name)
			                 .set_thumbnail(_artist_picture)
			                 .set_image(_album_picture));
	return result;
}