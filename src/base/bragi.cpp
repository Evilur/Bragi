#include "master.h"
#include "bragi.h"
#include "locale/locale.h"
#include "exception/bragi_exception.h"
#include "util/color.h"
#include "util/parser.h"

Bragi::Bragi(const dpp::snowflake &guild_id) : guild_id(guild_id) { }

dpp::message Bragi::NextCommand(const dpp::snowflake &channel_id, unsigned short track_index) {
	/* If the playlist is empty, throw an exception */
	if (IsEmpty()) throw BragiException(DIC_SLASH_NEXT_PLAYLIST_EMPTY, BragiException::SOFT);

	/* Get the index of the track */
	if (track_index == 0 || track_index > _tracks_size) track_index = _tracks_size - 1;
	else track_index--;

	/* If we need to replace the current playing track, abort it */
	bool is_playing = !track_index;
	if (is_playing) _tracks.Head()->Abort();

	/* Get the next track pointer */
	Track* &old_track = _tracks[track_index];
	Track* next_track = old_track->Next();

	/* If there is no new track */
	if (next_track == nullptr) throw BragiException(DIC_SLASH_NEXT_NO_RESULTS, BragiException::SOFT);

	/* Delete the old track and replace with the next one */
	delete old_track;
	old_track = next_track;

	/* If the old track is playing, stop the audio, clear the packet queue, and play the new track */
	if (is_playing) {
		_voiceclient->stop_audio();
		if (IsPlayerReady()) next_track->AsyncPlay(_voiceclient, _playback_rate);
	}

	/* Return the _message */
	return next_track->GetMessage(is_playing, channel_id);
}

std::string Bragi::Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id) {
	/* Get the user voice channel */
	const dpp::guild* const guild = dpp::find_guild(guild_id);
	const dpp::channel* const user_voice_channel = dpp::find_channel(guild->voice_members.find(user_id)->second.channel_id);

	/* If the user isn't in a voice channel */
	if (!user_voice_channel)
		throw BragiException(DIC_ERROR_USER_NOT_IN_VOICE_CHANNEL, BragiException::HARD);

	/* If the user and a bot already in the same channel */
	if (IsPlayerReady() && _voiceclient->channel_id == user_voice_channel->id)
		throw BragiException(DIC_ERROR_ALREADY_IN_CURRENT_CHANNEL, BragiException::SOFT);

	/* If bot can not connect to the channel or speak there */
	dpp::permission channel_permission = user_voice_channel->get_user_permissions(&bot->me);
	if (!channel_permission.can(dpp::p_connect) || !channel_permission.can(dpp::p_speak))
		throw BragiException(DIC_ERROR_PERMISSION_DENIED, BragiException::HARD);

	/* If we have a track in our playlist, abort it */
	if (!IsEmpty()) _tracks.Head()->Abort();

	/* Connect to the new channel */
	ds_client->connect_voice(guild_id, user_voice_channel->id);

	/* Reset the old voice connection */
	_voiceclient = nullptr;

	/* Return the result to the channel */
	return std::format(DIC_JOINED, user_voice_channel->name);
}

std::string Bragi::Leave(const dpp::snowflake &channel_id) {
	/* If the bot isn't in a voice channel */
	if (!_voiceclient)
		throw BragiException(DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL, BragiException::SOFT);

	/* If the playlist isn't empty, abort the first track to avoid sending the data to the old voice client */
	if (!IsEmpty()) _tracks.Head()->Abort();
	ds_client->disconnect_voice(guild_id);
	_voiceclient = nullptr;
	return DIC_LEFT;
}

void Bragi::HandleMarker() {
	/* Check the loop type */
	if (_loop_type == TRACK) _tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);
	else if (_loop_type == PLAYLIST) {
		/* Move the first track to the end of the playlist */
		Track* track = _tracks.Head();
		_tracks.PopFront();
		_tracks.Push(track);

		/* Play the next track */
		_tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);
	} else {
		/* Remove the first track in the list */
		_tracks.PopFront([](Track* track) { delete track; });
		_tracks_size--;

		/* If the playlist isn't empty, play the next track */
		if (!IsEmpty()) _tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);
	}
}

void Bragi::HandleVoiceStateUpdate(const dpp::snowflake &channel_id) {
	/* If the voice client isn't initialized, exit the method */
	if (!_voiceclient) return;

	/* If the voice channel doesn't change, exit the method */
	if (_voiceclient->channel_id == channel_id) return;

	/* If the playlist isn't empty, abort the first track to avoid sending the data to the old voice client */
	if (!IsEmpty()) _tracks.Head()->Abort();

	/* Reset the old voice connection */
	_voiceclient->stop_audio();
	_voiceclient = nullptr;

	/* If the bot reconnect to the other voice channelm connect to the new voice channel */
	if (channel_id) ds_client->connect_voice(guild_id, channel_id);
}

void Bragi::HandleReadyState(dpp::discord_voice_client* const voiceclient) {
	/* Update the voice */
	_voiceclient = voiceclient;

	/* Set the audio type to store data in the buffer before sending */
	_voiceclient->set_send_audio_type(dpp::discord_voice_client::send_audio_type_t::satype_recorded_audio);

	/* If we need to play the first track, play it */
	if (!IsEmpty()) _tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);
}

inline bool Bragi::IsPlayerReady() { return _voiceclient && _voiceclient->is_ready(); }

inline bool Bragi::IsEmpty() const { return !_tracks_size; }

Bragi* Bragi::Get(const dpp::snowflake &guild_id) {
	/* Try to get the guild in the list */
	for (Bragi* const player: _players)
		if (player->guild_id == guild_id) return player;

	/* If there is not a such guild player we need to add it to the array */
	return Add(guild_id);
}

Bragi* Bragi::Add(const dpp::snowflake &guild_id) {
	/* Create a new guild player */
	Bragi* player = new Bragi(guild_id);

	/* Add it to the list */
	_players.Push(player);

	/* Return a new player */
	return player;
}
