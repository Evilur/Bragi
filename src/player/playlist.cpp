#include "playlist.h"

void Playlist::Add(Track* track) {
	/* If there is enought space in the array */
	if (_track_count < _max_track_count) {
		_tracks[_track_count++] = track;
		return;
	}

	/* If there isn't enought space in the array */
	_max_track_count += TRACKS_DELTA;
	Track** tmp_tracks = new Track* [_max_track_count];
	for (unsigned short i = 0; i < _track_count; i++) tmp_tracks[i] = _tracks[i];
	delete[] _tracks;
	_tracks = tmp_tracks;
	_tracks[_track_count++] = track;
}

void Playlist::Skip() { }

bool Playlist::IsEmpty() { return _track_count == 0; }

unsigned short Playlist::GetSize() { return _track_count; }

Track* Playlist::operator[](unsigned short index) { return _tracks[index]; }