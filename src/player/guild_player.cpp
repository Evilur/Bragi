#include "guild_player.h"
#include "master.h"
#include "util/logger.h"
#include "coms/join.h"
#include "util/bragi_exception.h"

#include <opus/opus.h>
#include <opus/opusenc.h>

std::ifstream fs;
#define READ_SIZE 256

GuildPlayer::GuildPlayer(dpp::snowflake* guild_id) : guild_id(*guild_id) {
	this->voiceconn = ds_client->get_voice(*guild_id);
}

void GuildPlayer::PlayTrack(dpp::cluster &bot, const dpp::snowflake user_id, const dpp::snowflake channel_id, const Track *track) {
	FILE *fin;
	OggOpusEnc *enc;
	OggOpusComments *comments;
	int error;
	fin = fopen("/home/flame/Downloads/CHSV.wav", "rb");

	comments = ope_comments_create();
	ope_comments_add(comments, "ARTIST", "Someone");
	ope_comments_add(comments, "TITLE", "Some track");
	enc = ope_encoder_create_file("/home/flame/Downloads/FFF.opus", comments, 48000, 2, 0, &error);
	while (1) {
		short buf[2*READ_SIZE];
		int ret = fread(buf, 2*sizeof(short), READ_SIZE, fin);
		if (ret > 0) {
			ope_encoder_write(enc, buf, ret);
		} else break;
	}
	ope_encoder_drain(enc);
	ope_encoder_destroy(enc);
	ope_comments_destroy(comments);
	fclose(fin);
	throw BragiException("Успех!", channel_id, Hard);
}

bool GuildPlayer::IsPLayerReady() {
	return voiceconn != nullptr && voiceconn->voiceclient != nullptr && voiceconn->voiceclient->is_ready();
}

void GuildPlayer::ConnectVoice(dpp::snowflake channel_id) {
	ds_client->connect_voice(guild_id, channel_id);
}

void GuildPlayer::Reconnect() {
	voiceconn = ds_client->get_voice(guild_id);
}

GuildPlayer* GuildPlayer::Get(dpp::snowflake guild_id) {
	/* Try to get the guild in the array */
	for (unsigned int i = 0; i < _guild_count; i++)
		if (_guilds[i]->guild_id == guild_id) return _guilds[i];
	
	/* If there is not a such guild we need to add it to the array */
	return Add(guild_id);
}

GuildPlayer* GuildPlayer::Add(dpp::snowflake guild_id) {
	/* Increase the number of guilds */
	_guild_count++;
	
	/* Try to get the empty place for the pointer */
	for (unsigned int i = _max_guild_count - guilds_delta; i < _max_guild_count; i++) {
		if (_guilds[i] != nullptr) continue;
		_guilds[i] = new GuildPlayer(&guild_id);
		return _guilds[i];
	}
	
	/* If the array with pointer is full we need to increase it */
	auto** new_guilds = new GuildPlayer*[_max_guild_count + guilds_delta];
	for (unsigned int i = 0; i < _max_guild_count; i++) new_guilds[i] = _guilds[i];
	delete[] _guilds;
	_guilds = new_guilds;
	
	/* Add the new guild to the array */
	_guilds[_max_guild_count] = new GuildPlayer(&guild_id);
	return _guilds[_max_guild_count += guilds_delta];
}
