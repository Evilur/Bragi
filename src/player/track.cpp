#include "track.h"
#include "util/dictionary.h"
#include "util/color.h"

Track::~Track() { 
	delete _converter;
	_converter = nullptr;
}

std::string Track::GetTitle() { return _title; }

std::string Track::GetAlbum() { return _album; }

std::string Track::GetArtist() { return _artist; }

dpp::message Track::GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) {
	/* TEMPORARY REGION */
	_title = "Test title";
	_album = "Test album";
	_artist = "Test artist";
	_duration = "4m20c";
	/* END TEMP REGION */
	
	std::stringstream msg_body;
	if (is_playing_now) msg_body << std::format(DIC_TRACK_PLAYING_NOW, _title);
	else msg_body << std::format(DIC_TRACK_ADD_TO_PLAYLIST, _title);
	msg_body << '\n' << std::format(DIC_TRACK_DURATION, _duration);
	return dpp::message(channel_id, msg_body.str()).add_embed(dpp::embed()
																	.set_color(Color::RED)
																	.add_field(DIC_TRACK_ALBUM, _album)
																	.add_field(DIC_TRACK_ARTIST, _artist));
}