#include "master.h"
#include "guild_player.h"
#include "util/dictionary.h"
#include "exception/bragi_exception.h"
#include "util/logger.h"
#include "util/color.h"
#include "util/parser.h"

GuildPlayer::GuildPlayer(const dpp::snowflake &guild_id) : guild_id(guild_id) { }

dpp::message GuildPlayer::PlayCommand(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track* track) {
	bool need_to_play_first_track = IsEmpty();  //If the playlist is empty this track will be played right now
	dpp::message result_msg = track->GetMessage(need_to_play_first_track, channel_id);  //Get track message

	/* If player is ready */
	if (IsPlayerReady()) {
		/* Add a track to the playlist */
		_tracks.Push(track);
		_tracks_size++;

		if (need_to_play_first_track) track->AsyncPlay(_voiceconn, _speed_percent);  //Play the current track
		return result_msg;  //Return the track message
	}

	/* If player is not ready */
	result_msg.content.insert(0, Join(user_id, channel_id) + '\n');  //Join to the channel and insert the message to the result message

	/* Add a track to the playlist (if we successfully joined the channel) */
	_tracks.Push(track);
	_tracks_size++;
	return result_msg;  //Return a track message
}

dpp::message GuildPlayer::SkipCommand(const dpp::snowflake &channel_id, unsigned short num_for_skip) {
	/* If the playlist is empty */
	if (IsEmpty()) throw BragiException(DIC_SKIP_PLAYLIST_IS_EMPTY, channel_id, SOFT);

	/* If we can't skip that number of tracks */
	if (num_for_skip == 0) throw BragiException(DIC_SKIP_WRONG_NUM_FOR_SKIP, channel_id, SOFT);

	/* Stop the audio and clear the packet queue */
	_tracks.Head()->Abort();
	if (IsPlayerReady()) _voiceconn->voiceclient->stop_audio();

	/* Get the current track number for skip */
	if (num_for_skip > _tracks_size) num_for_skip = _tracks_size;

	/* Skip delete track/tracks from the playlist */
	_tracks.PopFront(num_for_skip, [](Track* track) { delete track; });
	_tracks_size -= num_for_skip;

	/* If the playlist isn't empty, play the next track */
	if (!IsEmpty()) _tracks.Head()->AsyncPlay(_voiceconn, _speed_percent);

	/* Return the message */
	return dpp::message(channel_id, std::format(DIC_SKIP_MSG, num_for_skip));
}

dpp::message GuildPlayer::SpeedCommand(const dpp::snowflake &channel_id, const byte speed_percent) {
	/* Set the speed percent */
	_speed_percent = speed_percent;

	/* Return a message */
	return dpp::message(channel_id, std::format(DIC_SLASH_SPEED_MSG, speed_percent));
}

dpp::message GuildPlayer::ListCommand(const dpp::snowflake &channel_id) {
	if (IsEmpty())
		return dpp::message(channel_id, dpp::embed()
				.set_color(Color::GREEN)
				.set_title(DIC_SLASH_LIST_MSG_EMPTY_TITLE));

	std::stringstream sstream;
	unsigned int duration = 0, counter = 1;

	for (const Track* const track: _tracks) {
		duration += track->GetDuration();
		sstream << counter++ << ". " << track->GetTrackData() << '\n';
	}

	return dpp::message(channel_id, dpp::embed()
			.set_color(Color::GREEN)
			.set_title(std::format(DIC_SLASH_LIST_MSG_TITLE, Parser::Time(duration)))
			.set_description(sstream.str()));
}

dpp::message GuildPlayer::LoopCommand(const dpp::snowflake &channel_id, const LoopType loop_type) {
	/* Update the loop type */
	_loop_type = loop_type;

	/* Send the reply message */
	if (loop_type == TRACK) return dpp::message(channel_id, DIC_SLASH_LOOP_TYPE_TRACK);
	else if (loop_type == PLAYLIST) return dpp::message(channel_id, DIC_SLASH_LOOP_TYPE_PLAYLIST);
	else return dpp::message(channel_id, DIC_SLASH_LOOP_TYPE_DISABLED);
}

dpp::message GuildPlayer::NextCommand(const dpp::snowflake &channel_id, unsigned short track_index) {
	/* If the playlist is empty, throw an exception */
	if (IsEmpty()) throw BragiException(DIC_SLASH_NEXT_PLAYLIST_EMPTY, channel_id, SOFT);

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
	if (next_track == nullptr) throw BragiException(DIC_SLASH_NEXT_NO_RESULTS, channel_id, SOFT);

	/* Delete the old track and replace with the next one */
	delete old_track;
	old_track = next_track;

	/* If the old track is playing, stop the audio, clear the packet queue, and play the new track */
	if (is_playing) {
		_voiceconn->voiceclient->stop_audio();
		next_track->AsyncPlay(_voiceconn, _speed_percent);
	}

	/* Return the message */
	return next_track->GetMessage(is_playing, channel_id);
}

std::string GuildPlayer::Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id) {
	/* Get voice channels */
	dpp::guild* guild = dpp::find_guild(guild_id);
	dpp::channel* bot_vc = dpp::find_channel(guild->voice_members.find(bot->me.id)->second.channel_id);
	dpp::channel* user_vc = dpp::find_channel(guild->voice_members.find(user_id)->second.channel_id);

	/* If the user isn't in a voice channel */
	if (user_vc == nullptr)
		throw BragiException(DIC_ERROR_USER_NOT_IN_VOICE_CHANNEL, channel_id, HARD);

	/* If the user and a bot already in the same channel */
	if (IsPlayerReady() && bot_vc != nullptr && bot_vc->id == user_vc->id)
		throw BragiException(DIC_ERROR_ALREADY_IN_CURRENT_CHANNEL, channel_id, SOFT);

	/* If bot can not connect to the channel or speak there */
	if (!user_vc->get_user_permissions(&bot->me).can(dpp::p_connect) || !user_vc->get_user_permissions(&bot->me).can(dpp::p_speak))
		throw BragiException(DIC_ERROR_PERMISSION_DENIED, channel_id, HARD);

	/* If bot in the voice channel we need to disconnect */
	if (bot_vc != nullptr) ds_client->disconnect_voice(guild_id);

	/* If all is OK */
	ds_client->connect_voice(guild_id, user_vc->id);
	return std::format(DIC_JOINED, user_vc->name);
}

std::string GuildPlayer::Leave(const dpp::snowflake &channel_id) {
	/* If the bot isn't in a voice channel */
	if (_voiceconn == nullptr)
		throw BragiException(DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL, channel_id, SOFT);

	ds_client->disconnect_voice(guild_id);
	_voiceconn = nullptr;
	return DIC_LEFT;
}

void GuildPlayer::HandleMarker() {
	/* Check the loop type */
	if (_loop_type == TRACK) _tracks.Head()->AsyncPlay(_voiceconn, _speed_percent);
	else if (_loop_type == PLAYLIST) {
		/* Move the first track to the end of the playlist */
		Track* track = _tracks.Head();
		_tracks.PopFront();
		_tracks.Push(track);

		/* Play the next track */
		_tracks.Head()->AsyncPlay(_voiceconn, _speed_percent);
	} else {
		/* Remove the first track in the list */
		_tracks.PopFront([](Track* track) { delete track; });
		_tracks_size--;

		/* If the playlist isn't empty, play the next track */
		if (!IsEmpty()) _tracks.Head()->AsyncPlay(_voiceconn, _speed_percent);
	}
}

void GuildPlayer::HandleReadyState() {
	/* Update the voice */
	_voiceconn = ds_client->get_voice(guild_id);
	_voiceconn->voiceclient->set_send_audio_type(dpp::discord_voice_client::send_audio_type_t::satype_recorded_audio);

	/* If we need to play the first track */
	if (!IsEmpty()) _tracks.Head()->AsyncPlay(_voiceconn, _speed_percent);
}

GuildPlayer* GuildPlayer::Get(const dpp::snowflake &guild_id) {
	/* Try to get the guild in the list */
	for (GuildPlayer* const player: _players)
		if (player->guild_id == guild_id) return player;

	/* If there is not a such guild player we need to add it to the array */
	return Add(guild_id);
}

bool GuildPlayer::IsPlayerReady() { return _voiceconn != nullptr && _voiceconn->voiceclient != nullptr && _voiceconn->voiceclient->is_ready(); }

inline bool GuildPlayer::IsEmpty() const { return !_tracks_size; }

GuildPlayer* GuildPlayer::Add(const dpp::snowflake &guild_id) {
	/* Create a new guild player */
	GuildPlayer* player = new GuildPlayer(guild_id);

	/* Add it to the list */
	_players.Push(player);

	/* Return a new player */
	return player;
}