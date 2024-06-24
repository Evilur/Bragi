#include "master.h"
#include "guild_player.h"
#include "util/dictionary.h"
#include "exception/bragi_exception.h"
#include "converter/audio_to_opus.h"
#include "util/logger.h"
#include "util/color.h"

GuildPlayer::GuildPlayer(const dpp::snowflake &guild_id) : guild_id(guild_id) {
	this->_voiceconn = ds_client->get_voice(guild_id);
}

dpp::message GuildPlayer::HandleTrack(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track* track) {
	bool is_playing_now = _playlist.IsEmpty();  //If the playlist is empty this track will be played right now
	_playlist.Add(track);  //Add a track to the playlist
	dpp::message result_msg = track->GetMessage(is_playing_now, channel_id);  //Get track message

	/* If player is ready */
	if (IsPlayerReady()) {
		/* If we need to play the track right now */
		if (is_playing_now) _voiceconn->voiceclient->insert_marker("PFT");  //Insert a marker to the voiceclient for call the 'on_voice_track_marker' event
		return result_msg;  //Return a track message
	}

	/* If player is not ready */
	result_msg.content.insert(0, Join(user_id, channel_id) + '\n');  //Join to the channel and insert it to the track message
	return result_msg;  //Return a track message
}

dpp::message GuildPlayer::GetPlaylistMessage(const dpp::snowflake &channel_id) {
	std::stringstream ss("**");
	int duration = 0;

	for (unsigned short i = 0; i < _playlist.GetSize(); i++) {
		duration += _playlist[i]->GetDuration();
		ss << i + 1 << ". " << _playlist[i]->GetTitle() << '\n';
	}

	ss << "**";

	return dpp::message(channel_id, dpp::embed()
			.set_color(Color::GREEN)
			.set_title(std::format(DIC_SLASH_LIST_MSG_TITLE, duration))
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

void GuildPlayer::HandleMarker(const std::string &meta) {
	/* meta can be "PFT" (play first track) or "EOF" (end of file) */
	if (meta == "PFT") SendOpus(_playlist[0]);  //If we need to play first track
	else _playlist.Skip();  //If there is the end of the file
}

void GuildPlayer::HandleReadyState() {
	_voiceconn = ds_client->get_voice(guild_id);  //Update the voice
	if (!_playlist.IsEmpty()) SendOpus(_playlist[0]);  //If the playlist is not empty play the first track
}

GuildPlayer* GuildPlayer::Get(const dpp::snowflake &guild_id) {
	/* Try to get the guild in the array */
	for (unsigned int i = 0; i < _players_count; i++)
		if (_players[i]->guild_id == guild_id) return _players[i];

	/* If there is not a such guild we need to add it to the array */
	return Add(guild_id);
}

void GuildPlayer::SendOpus(Track* track) {
	unsigned char chunk[AudioToOpus::OPUS_CHUNK_SIZE];
	while (track->CanRead()) {
		int len = track->GetOpus(chunk);
		_voiceconn->voiceclient->send_audio_opus(chunk, len);
	}
	_voiceconn->voiceclient->insert_marker("EOF");
}

bool GuildPlayer::IsPlayerReady() {
	return _voiceconn != nullptr && _voiceconn->voiceclient != nullptr && _voiceconn->voiceclient->is_ready();
}

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