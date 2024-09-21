#include "playlist.h"
#include "util/color.h"
#include "util/dictionary.h"
#include "util/parser.h"
#include "util/logger.h"

void Playlist::Add(Track* track) {
	/* Create an array pointer with the right offset */
	Track** tracks_ptr = _tracks + _tracks_offset;

	/* If there is enought space in the array, just add the track to the array */
	if (_tracks_offset + _tracks_size < _max_track_size) {
		tracks_ptr[_tracks_size++] = track;
		return;
	}

	/* If we have an offset in the array */
	if (_tracks_offset != 0) {
		Logger::Debug("Adding with the offset");

		/* Remove the offset */
		std::copy(tracks_ptr, tracks_ptr + _tracks_size, _tracks);
		tracks_ptr = _tracks;

		/* Add the track to the array */
		tracks_ptr[_tracks_size++] = track;
		return;
	}

	/* If the array is full of tracks */
	Logger::Fatal("Not implemented yet");
	exit(105);
}

void Playlist::Skip() {
	/* If there isn't tracks in the array, reset the offset */
	if (--_tracks_size == 0) _tracks_offset = 0;

	/* Free memory of the listened track */
	delete _tracks[_tracks_offset];
	_tracks[_tracks_offset++] = nullptr;
}

bool Playlist::IsEmpty() { return _tracks_size == 0; }

Track* Playlist::CurrentTrack() { return _tracks[_tracks_offset]; }

dpp::message Playlist::Message(const dpp::snowflake &channel_id) {
	if (_tracks_size == 0)
		return dpp::message(channel_id, dpp::embed()
				.set_color(Color::GREEN)
				.set_title(DIC_SLASH_LIST_MSG_EMPTY_TITLE));

	std::stringstream ss;
	int duration = 0;

	ss << "**";

	Track** tracks_ptr = _tracks + _tracks_offset;
	for (unsigned short i = 0; i < _tracks_size; i++) {
		duration += tracks_ptr[i]->GetDuration();
		ss << i + 1 << ". " << tracks_ptr[i]->GetTrackData() << '\n';
	}

	ss << "**";

	return dpp::message(channel_id, dpp::embed()
			.set_color(Color::GREEN)
			.set_title(std::format(DIC_SLASH_LIST_MSG_TITLE, Parser::Time(duration)))
			.set_description(ss.str()));
}