#ifndef BRAGI_PLAYLIST_H
#define BRAGI_PLAYLIST_H

#include "track.h"

class Playlist final {
public:
	void Add(Track* track);
	unsigned short GetSize();
private:
	static inline constexpr unsigned short TRACKS_DELTA = 64;
	unsigned short _max_track_count = TRACKS_DELTA;
	unsigned short _track_count = 0;
	Track** _tracks = new Track*[TRACKS_DELTA];
};

#endif
