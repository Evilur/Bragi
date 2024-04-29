#ifndef BRAGI_GUILD_PLAYER_H
#define BRAGI_GUILD_PLAYER_H

#include "track.h"
#include "playlist.h"

#include <dpp/dpp.h>

class GuildPlayer final {
public:
	const dpp::snowflake guild_id;
	
	explicit GuildPlayer(const dpp::snowflake &guild_id);
	
	dpp::message HandleTrack(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track* track);
	dpp::message Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id);
	dpp::message Leave(const dpp::snowflake &channel_id);
	
	void UpdateVoice();
	void EndOfTrack();

	static GuildPlayer* Get(const dpp::snowflake &guild_id);
private:
	bool _need_to_play_first_track = false;
	dpp::voiceconn* _voiceconn;
	Playlist _playlist;
	
	void SendOpus(Track* track);
	bool PlayFirstTrack();
	bool IsPlayerReady();

	static inline constexpr unsigned short GUILDS_DELTA = 4;
	static inline unsigned int _max_guild_count = GUILDS_DELTA;
	static inline unsigned int _guild_count = 0;
	static inline GuildPlayer** _guilds = new GuildPlayer*[GUILDS_DELTA];

	static GuildPlayer* Add(const dpp::snowflake &guild_id);
};

#endif
