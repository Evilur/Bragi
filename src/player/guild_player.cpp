#include "master.h"
#include "guild_player.h"
#include "util/dictionary.h"
#include "exception/bragi_exception.h"
#include "util/logger.h"
#include "util/color.h"
#include "util/parser.h"

#include <thread>

GuildPlayer::GuildPlayer(const dpp::snowflake &guild_id) : guild_id(guild_id) { }

dpp::message GuildPlayer::HandleTrack(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track* track) {
	_need_to_play_first_track = _playlist.IsEmpty();  //If the playlist is empty this track will be played right now
	_playlist.Add(track);  //Add a track to the playlist
	dpp::message result_msg = track->GetMessage(_need_to_play_first_track, channel_id);  //Get track message

	/* If player is ready */
	if (IsPlayerReady()) {
		/* If we need to play the track right now */
		if (_need_to_play_first_track) track->AsyncPlay(_voiceconn);
		return result_msg;  //Return a track message
	}

	/* If player is not ready */
	result_msg.content.insert(0, Join(user_id, channel_id) + '\n');  //Join to the channel and insert ther message to the track message
	return result_msg;  //Return a track message
}

dpp::message GuildPlayer::GetPlaylistMessage(const dpp::snowflake &channel_id) {
	if (_playlist.IsEmpty())
		return dpp::message(channel_id, dpp::embed()
				.set_color(Color::GREEN)
				.set_title(DIC_SLASH_LIST_MSG_EMPTY_TITLE));

	std::stringstream ss;
	int duration = 0;

	ss << "**";

	for (unsigned short i = 0; i < _playlist.GetSize(); i++) {
		duration += _playlist[i]->GetDuration();
		ss << i + 1 << ". " << _playlist[i]->GetTrackData() << '\n';
	}

	ss << "**";

	return dpp::message(channel_id, dpp::embed()
			.set_color(Color::GREEN)
			.set_title(std::format(DIC_SLASH_LIST_MSG_TITLE, Parser::Time(duration)))
			.set_description(ss.str()));
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

dpp::message GuildPlayer::Leave(const dpp::snowflake &channel_id) {
	/* If the bot isn't in a voice channel */
	if (_voiceconn == nullptr)
		throw BragiException(DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL, channel_id, SOFT);

	ds_client->disconnect_voice(guild_id);
	_voiceconn = nullptr;
	return dpp::message(channel_id, DIC_LEFT);
}

void GuildPlayer::HandleMarker() {
	/* If we touch the marker, the track has ended */
	_playlist.Skip();
}

void GuildPlayer::HandleReadyState() {
	/* Update the voice */
	_voiceconn = ds_client->get_voice(guild_id);
	_voiceconn->voiceclient->set_send_audio_type(dpp::discord_voice_client::send_audio_type_t::satype_recorded_audio);

	/* If we need to play the first track */
	if (_need_to_play_first_track) {
		_playlist[0]->AsyncPlay(_voiceconn);
		_need_to_play_first_track = false;
	}
}

GuildPlayer* GuildPlayer::Get(const dpp::snowflake &guild_id) {
	/* Try to get the guild in the array */
	for (unsigned int i = 0; i < _players_count; i++)
		if (_players[i]->guild_id == guild_id) return _players[i];

	/* If there is not a such guild we need to add it to the array */
	return Add(guild_id);
}

bool GuildPlayer::IsPlayerReady() { return _voiceconn != nullptr && _voiceconn->voiceclient != nullptr && _voiceconn->voiceclient->is_ready(); }

GuildPlayer* GuildPlayer::Add(const dpp::snowflake &guild_id) {
	/* Increase the number of guilds */
	_players_count++;

	/* Try to get the empty place for the pointer */
	for (unsigned int i = _max_players_count - PLAYERS_DELTA; i < _max_players_count; i++) {
		if (_players[i] != nullptr) continue;
		_players[i] = new GuildPlayer(guild_id);
		return _players[i];
	}

	/* If the array with pointer is full we need to increase it */
	auto** new_guilds = new GuildPlayer* [_max_players_count + PLAYERS_DELTA];
	for (unsigned int i = 0; i < _max_players_count; i++) new_guilds[i] = _players[i];
	delete[] _players;
	_players = new_guilds;

	/* Add the new guild to the array */
	_players[_max_players_count] = new GuildPlayer(guild_id);
	return _players[_max_players_count += PLAYERS_DELTA];
}