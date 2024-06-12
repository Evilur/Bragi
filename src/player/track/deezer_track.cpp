#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-pass-by-value"

#include "deezer_track.h"
#include "util/num_parser.h"

int DeezerTrack::GetOpus(unsigned char* out) {
	return 0;
}

bool DeezerTrack::CanRead() {
	return false;
}

DeezerTrack::DeezerTrack(const std::string &id, const std::string &album_id, const std::string &artist_id,
                         const std::string &title, const std::string &album_title, const std::string &artist_name,
                         const std::string &album_picture, const std::string &artist_picture,
                         const std::string &duration, const std::string &url,
                         const unsigned short &total, const unsigned short &next) :
		_id(NumParser::ToUInt32(id.c_str())), _album_id(NumParser::ToUInt32(album_id.c_str())), _artist_id(NumParser::ToUInt32(artist_id.c_str())),
		_title(title), _album_title(album_title), _artist_name(artist_name),
		_album_picture("http://e-cdns-images.dzcdn.net/images/cover/" + album_picture + "/1000x1000-000000-80-0-0.jpg"),
		_artist_picture("http://e-cdns-images.dzcdn.net/images/artist/" + artist_picture + "/1000x1000-000000-80-0-0.jpg"),
		_duration(NumParser::ToUInt16(duration.c_str())), _url(url),
		_total(total), _next(next) { }

#pragma clang diagnostic pop