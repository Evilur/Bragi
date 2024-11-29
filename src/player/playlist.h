#ifndef BRAGI_PLAYLIST_H
#define BRAGI_PLAYLIST_H

#include "player/track/track.h"
#include "master.h"

class Playlist final {
public:
	void Add(Track* track);

	void HandleEof();

	unsigned short Skip(const unsigned short num_for_skip);

	void RepeatPlaylist();

	Track* Next(const unsigned short track_index, bool &is_playing);

	bool IsEmpty() const;

	unsigned short Size() const;

	Track* CurrentTrack() const;

	dpp::message Message(const dpp::snowflake &channel_id) const;

private:
	static inline constexpr unsigned short DEFAULT_TRACKS_SIZE = 32;
	unsigned short _max_track_size = DEFAULT_TRACKS_SIZE;
	unsigned short _tracks_size = 0;
	unsigned short _tracks_offset = 0;
	Track** _tracks = new Track*[DEFAULT_TRACKS_SIZE];

	void ResetArray();
};

#endif