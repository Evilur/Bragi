#include "guild_player.h"
#include "master.h"
#include "util/logger.h"
#include "util/dictionary.h"
#include "exception/bragi_exception.h"
#include "converter/audio_to_opus.h"

GuildPlayer::GuildPlayer(const dpp::snowflake &guild_id) : guild_id(guild_id) {
	this->_voiceconn = ds_client->get_voice(guild_id);
}

dpp::message GuildPlayer::HandleTrack(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track *track) {
	unsigned char chunk[AudioToOpus::OPUS_CHUNK_SIZE];
	while (track->CanRead()) {
		int len = track->GetOpus(chunk);
		_voiceconn->voiceclient->send_audio_opus(chunk, len);
	}

	throw BragiException("Успех!", channel_id, HARD);
}

dpp::message GuildPlayer::Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id) {
	/* Get voice channels */
	dpp::guild* guild = dpp::find_guild(guild_id);
	dpp::channel* bot_vc = dpp::find_channel(guild->voice_members.find(bot->me.id)->second.channel_id);
	dpp::channel* user_vc = dpp::find_channel(guild->voice_members.find(user_id)->second.channel_id);

	/* If the user isn't in a voice channel */
	if (user_vc == nullptr)
		throw BragiException(DIC_ERROR_USER_NOT_IN_VOICE_CHANNEL, channel_id, HARD);

	/* If the user and a bot already in the same channel */
	if (IsPLayerReady() && bot_vc != nullptr && bot_vc->id == user_vc->id)
		throw BragiException(DIC_ERROR_ALREADY_IN_CURRENT_CHANNEL, channel_id, SOFT);

	/* If bot can not connect to the channel or speak there */
	if (!user_vc->get_user_permissions(&bot->me).can(dpp::p_connect) || !user_vc->get_user_permissions(&bot->me).can(dpp::p_speak))
		throw BragiException(DIC_ERROR_PERMISSION_DENIED, channel_id, HARD);

	/* If bot in the voice channel we need to disconnect */
	if (bot_vc != nullptr) ds_client->disconnect_voice(guild_id);

	/* If all is OK */
	ds_client->connect_voice(guild_id, user_vc->id);
	return dpp::message(channel_id, std::format(DIC_JOINED, user_vc->name));
}

dpp::message GuildPlayer::Leave(const dpp::snowflake &channel_id) {
	/* If the bot isn't in a voice channel */
	if (_voiceconn == nullptr)
		throw BragiException(DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL, channel_id, SOFT);

	ds_client->disconnect_voice(guild_id);
	_voiceconn = nullptr;
	return dpp::message(channel_id, DIC_LEFT);
}

void GuildPlayer::Reconnect() {
	_voiceconn = ds_client->get_voice(guild_id);
}

GuildPlayer* GuildPlayer::Get(const dpp::snowflake &guild_id) {
	/* Try to get the guild in the array */
	for (unsigned int i = 0; i < _guild_count; i++)
		if (_guilds[i]->guild_id == guild_id) return _guilds[i];
	
	/* If there is not a such guild we need to add it to the array */
	return Add(guild_id);
}

bool GuildPlayer::IsPLayerReady() {
	return _voiceconn != nullptr && _voiceconn->voiceclient != nullptr && _voiceconn->voiceclient->is_ready();
}

GuildPlayer* GuildPlayer::Add(const dpp::snowflake &guild_id) {
	/* Increase the number of guilds */
	_guild_count++;
	
	/* Try to get the empty place for the pointer */
	for (unsigned int i = _max_guild_count - GUILDS_DELTA; i < _max_guild_count; i++) {
		if (_guilds[i] != nullptr) continue;
		_guilds[i] = new GuildPlayer(guild_id);
		return _guilds[i];
	}
	
	/* If the array with pointer is full we need to increase it */
	auto** new_guilds = new GuildPlayer*[_max_guild_count + GUILDS_DELTA];
	for (unsigned int i = 0; i < _max_guild_count; i++) new_guilds[i] = _guilds[i];
	delete[] _guilds;
	_guilds = new_guilds;
	
	/* Add the new guild to the array */
	_guilds[_max_guild_count] = new GuildPlayer(guild_id);
	return _guilds[_max_guild_count += GUILDS_DELTA];
}
