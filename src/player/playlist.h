#ifndef BRAGI_PLAYLIST_H
#define BRAGI_PLAYLIST_H

#include "player/track/track.h"
#include "master.h"

class Playlist final {
public:
	void Add(Track* track);

	void Skip();

	bool IsEmpty();

	Track* CurrentTrack();

	dpp::message Message(const dpp::snowflake &channel_id);

private:
	static inline constexpr unsigned short TRACKS_DELTA = 32;
	unsigned short _max_track_size = TRACKS_DELTA;
	unsigned short _tracks_size = 0;
	unsigned short _tracks_offset = 0;
	Track** _tracks = new Track* [TRACKS_DELTA];
};

#endif