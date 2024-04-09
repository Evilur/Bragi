#include "guild_player.h"
#include "master.h"
#include "util/logger.h"
#include "coms/join.h"
#include "util/bragi_exception.h"

#include <opus/opus.h>

GuildPlayer::GuildPlayer(const dpp::snowflake* guild_id) : guild_id(*guild_id) {
	this->voiceconn = ds_client->get_voice(*guild_id);
}

void GuildPlayer::PlayTrack(dpp::cluster &bot, const dpp::snowflake user_id, const dpp::snowflake channel_id, const Track *track) {
	std::ifstream input("/home/flame/Downloads/CHSV.wav");
	
	const int freq = 48000;
	const int frame_size = 960;
	const int channels = 2;
	
	OpusEncoder* encoder = opus_encoder_create(freq, channels, OPUS_APPLICATION_AUDIO, nullptr);
	
	const int pcm_chunk_size = frame_size * channels * sizeof(opus_int16);
	char* pcm_chunk = new char[pcm_chunk_size];
	
	const int chunk_size = 16384;
	auto* chunk = new unsigned char[chunk_size];
	
	while (input.peek() != EOF) {
		input.read(pcm_chunk, pcm_chunk_size);
		int len = opus_encode(encoder, reinterpret_cast<const opus_int16 *>(pcm_chunk), frame_size, chunk, chunk_size);
		voiceconn->voiceclient->send_audio_opus((uint8_t*)chunk, len);
		
		delete[] pcm_chunk;
		pcm_chunk = new char[pcm_chunk_size];
		delete[] chunk;
		chunk = new unsigned char[chunk_size];
	}

	opus_encoder_destroy(encoder);
	throw BragiException("Успех!", channel_id, Hard);
}

bool GuildPlayer::IsPLayerReady() {
	return voiceconn != nullptr && voiceconn->voiceclient != nullptr && voiceconn->voiceclient->is_ready();
}

void GuildPlayer::ConnectVoice(const dpp::snowflake &channel_id) {
	ds_client->connect_voice(guild_id, channel_id);
}

void GuildPlayer::Reconnect() {
	voiceconn = ds_client->get_voice(guild_id);
}

GuildPlayer* GuildPlayer::Get(const dpp::snowflake &guild_id) {
	/* Try to get the guild in the array */
	for (unsigned int i = 0; i < _guild_count; i++)
		if (_guilds[i]->guild_id == guild_id) return _guilds[i];
	
	/* If there is not a such guild we need to add it to the array */
	return Add(guild_id);
}

GuildPlayer* GuildPlayer::Add(const dpp::snowflake &guild_id) {
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
