#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-pass-by-value"

#include "deezer_track.h"

int DeezerTrack::GetOpus(unsigned char* out) {
	return 0;
}

bool DeezerTrack::CanRead() {
	return false;
}

DeezerTrack::DeezerTrack(const unsigned int &id, const unsigned int &album_id, const unsigned int &artist_id,
                         const std::string &title, const std::string &album_title, const std::string &artist_name,
                         const std::string &album_picture, const std::string &artist_picture,
                         const std::string &token, const unsigned short &duration,
                         const unsigned short &total, const unsigned short &next) :
		_id(id), _album_id(album_id), _artist_id(artist_id),
		_title(title), _album_title(album_title), _artist_name(artist_name),
		_album_picture(album_picture), _artist_picture(artist_picture),
		_token(token), _duration(duration),
		_total(total), _next(next) { }

#pragma clang diagnostic pop