#include "playlist.h"
#include "util/color.h"
#include "util/dictionary.h"
#include "util/parser.h"
#include "util/logger.h"
#include "exception/bragi_exception.h"

void Playlist::Add(Track* track) {
	/* If there is enought space in the array, just add the track to the array */
	if (_tracks_offset + _tracks_size < _max_track_size) {
		_tracks[_tracks_offset + _tracks_size++] = track;
		return;
	}

	/* If we have an offset in the array, remove it */
	if (_tracks_offset != 0) {
		/* Create an array pointer with the right offset */
		Track** tracks_ptr = _tracks + _tracks_offset;

		/* Move all pointers to the start of the array */
		std::copy(tracks_ptr, tracks_ptr + _tracks_size, _tracks);

		/* Update the tracks pointer and remove the offset */
		tracks_ptr = _tracks;

		/* Reset the tracks offset */
		_tracks_offset = 0;

		/* Add the track to the array */
		tracks_ptr[_tracks_size++] = track;

		/* Replace old pointers in the array with nullptrs */
		std::fill(tracks_ptr + _tracks_size, tracks_ptr + _max_track_size, nullptr);
		return;
	}

	/* If the array is full of tracks, create a new array and copy all pointers there */
	Track** new_tracks_ptr = new Track*[_max_track_size + DEFAULT_TRACKS_SIZE];
	std::copy(_tracks, _tracks + _max_track_size, new_tracks_ptr);
	_max_track_size += DEFAULT_TRACKS_SIZE;

	/* Clear the old memory, and replace the old pointer with new ones */
	delete[] _tracks;
	_tracks = new_tracks_ptr;

	/* Add a track to the new array */
	_tracks[_tracks_size++] = track;
}

void Playlist::HandleEof() {
	/* Free memory of the listened track */
	delete _tracks[_tracks_offset];
	_tracks[_tracks_offset++] = nullptr;

	/* If there isn't tracks in the array, reset it */
	if (--_tracks_size == 0) ResetArray();
}

unsigned short Playlist::Skip(unsigned short num_for_skip) {
	/* Create an array pointer with the right offset */
	Track** tracks_ptr = _tracks + _tracks_offset;

	/* Abort the current playing track */
	tracks_ptr[0]->Abort();

	/* Delete skipped tracks */
	if (_tracks_size < num_for_skip) num_for_skip = _tracks_size;
	for (unsigned short i = 0; i < num_for_skip; i++) {
		delete tracks_ptr[i];
		tracks_ptr[i] = nullptr;
	}

	/* Set the track array size, offset and reset the array if there is no tracks in it */
	_tracks_size -= num_for_skip;
	if (_tracks_size == 0) ResetArray();
	else _tracks_offset += num_for_skip;

	/* Return the number of skipped tracks */
	return num_for_skip;
}

void Playlist::RepeatPlaylist() {
	Track* current_track = _tracks[_tracks_offset];
	_tracks[_tracks_offset++] = nullptr;
	_tracks_size--;
	Add(current_track);
}

Track* Playlist::Next(const unsigned short track_index, bool &is_playing) {
	/* Create an array pointer with the right offset */
	Track** tracks_ptr = _tracks + _tracks_offset;

	/* Get the pointer to the next track in the search results */
	Track* next_track = tracks_ptr[track_index]->Next();

	/* If there is no next track */
	if (next_track == nullptr) return nullptr;

	/* If we need to replace the current playing track, abort it */
	is_playing = !track_index;
	if (is_playing) tracks_ptr[0]->Abort();

	/* Free memory and replace the pointer */
	delete tracks_ptr[track_index];
	tracks_ptr[track_index] = next_track;

	/* Return the pointer to the next track */
	return next_track;
}

bool Playlist::IsEmpty() const { return _tracks_size == 0; }

unsigned short Playlist::Size() const { return _tracks_size; }

Track* Playlist::CurrentTrack() const { return _tracks[_tracks_offset]; }

dpp::message Playlist::Message(const dpp::snowflake &channel_id) const {
	if (_tracks_size == 0)
		return dpp::message(channel_id, dpp::embed()
				.set_color(Color::GREEN)
				.set_title(DIC_SLASH_LIST_MSG_EMPTY_TITLE));

	std::stringstream ss;
	int duration = 0;

	Track** tracks_ptr = _tracks + _tracks_offset;
	for (unsigned short i = 0; i < _tracks_size; i++) {
		duration += tracks_ptr[i]->GetDuration();
		ss << i + 1 << ". " << tracks_ptr[i]->GetTrackData() << '\n';
	}

	return dpp::message(channel_id, dpp::embed()
			.set_color(Color::GREEN)
			.set_title(std::format(DIC_SLASH_LIST_MSG_TITLE, Parser::Time(duration)))
			.set_description(ss.str()));
}

void Playlist::ResetArray() {
	/* Reset the offset */
	_tracks_offset = 0;

	/* If the array size is default, exit the method */
	if (_max_track_size == DEFAULT_TRACKS_SIZE) return;

	/* If the array is bigger than the default, free old memory */
	delete[] _tracks;

	/* Create a new array */
	_tracks = new Track*[DEFAULT_TRACKS_SIZE];
}