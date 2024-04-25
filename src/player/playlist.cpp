#include "playlist.h"

void Playlist::Add(Track* track) {
	_tracks[_track_count++] = track;
}

void Playlist::Skip() {
	_track_count = 0;
}

unsigned short Playlist::GetSize() { return _track_count; }

Track* Playlist::operator[](unsigned short index) {
	return _tracks[0];
}
