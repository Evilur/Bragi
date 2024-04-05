#include "guild.h"
#include "master.h"
#include "util/logger.h"
#include "coms/join.h"

#include <oggz/oggz.h>

Guild::Guild(dpp::snowflake* guild_id) : guild_id(*guild_id) {
	this->voiceconn = ds_client->get_voice(*guild_id);
}

dpp::message Guild::PlayTrack(dpp::cluster &bot, const dpp::snowflake user_id, const dpp::snowflake channel_id, const Track *track) {
	/* If the voice channel was invalid, or there is an issue with it, reconnect to the channel */
	if (!IsPLayerReady()) {
		bool joined;
		dpp::message join_msg = Join::Message(bot, guild_id, user_id, channel_id, &joined);
		if (!joined) return join_msg;
	}

	/* load the audio file with oggz */
	OGGZ *track_og = oggz_open("/home/flame/Downloads/CHSV.opus", OGGZ_READ);

	/* set read callback, this callback will be called on packets with the serialno,
	 * -1 means every packet will be handled with this callback.
	 */
	oggz_set_read_callback(
			track_og, -1,
			[](OGGZ *oggz, oggz_packet *packet, long serialno,
			   void *user_data) {
				dpp::voiceconn *voiceconn = (dpp::voiceconn *)user_data;

				/* send the audio */
				voiceconn->voiceclient->send_audio_opus(packet->op.packet,
				                                        packet->op.bytes);

				/* make sure to always return 0 here, read more on oggz documentation */
				return 0;
			},
			/* this will be the value of void *user_data */
			(void *)voiceconn
	);

	// read loop
	while (voiceconn && voiceconn->voiceclient && !voiceconn->voiceclient->terminating) {
		/* you can tweak this to whatever. Here I use BUFSIZ, defined in
		 * stdio.h as 8192.
		 */
		static const constexpr long CHUNK_READ = BUFSIZ * 2;

		const long read_bytes = oggz_read(track_og, CHUNK_READ);

		/* break on eof */
		if (!read_bytes) {
			break;
		}
	}

	/* Don't forget to free the memory */
	oggz_close(track_og);
	return nullptr;
}

bool Guild::IsPLayerReady() {
	return voiceconn != nullptr && voiceconn->voiceclient != nullptr && voiceconn->voiceclient->is_ready();
}

void Guild::ReconnectVoice() {
	voiceconn = ds_client->get_voice(guild_id);
}

Guild* Guild::Get(dpp::snowflake guild_id) {
	/* Try to get the guild in the array */
	for (unsigned int i = 0; i < _guild_count; i++)
		if (_guilds[i]->guild_id == guild_id) return _guilds[i];
	
	/* If there is not a such guild we need to add it to the array */
	return Add(guild_id);
}

Guild* Guild::Add(dpp::snowflake guild_id) {
	/* Increase the number of guilds */
	_guild_count++;
	
	/* Try to get the empty place for the pointer */
	for (unsigned int i = _max_guild_count - guilds_delta; i < _max_guild_count; i++) {
		if (_guilds[i] != nullptr) continue;
		_guilds[i] = new Guild(&guild_id);
		return _guilds[i];
	}
	
	/* If the array with pointer is full we need to increase it */
	auto** new_guilds = new Guild*[_max_guild_count + guilds_delta];
	for (unsigned int i = 0; i < _max_guild_count; i++) new_guilds[i] = _guilds[i];
	delete[] _guilds;
	_guilds = new_guilds;
	
	/* Add the new guild to the array */
	_guilds[_max_guild_count] = new Guild(&guild_id);
	return _guilds[_max_guild_count += guilds_delta];
}