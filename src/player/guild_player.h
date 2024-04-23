#ifndef BRAGI_GUILD_PLAYER_H
#define BRAGI_GUILD_PLAYER_H

#include "track.h"

#include <dpp/dpp.h>

class GuildPlayer final {
public:
	const dpp::snowflake guild_id;
	
	explicit GuildPlayer(const dpp::snowflake &guild_id);
	
	dpp::message HandleTrack(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track* track);
	dpp::message Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id);
	dpp::message Leave(const dpp::snowflake &channel_id);

	void Reconnect();
	
	static GuildPlayer* Get(const dpp::snowflake &guild_id);
private:
	dpp::voiceconn* _voiceconn;

	void SendOpus(Track* track);
	
	bool IsPLayerReady();
	
	static inline const unsigned int GUILDS_DELTA = 8;
	static inline unsigned int _max_guild_count = GUILDS_DELTA;
	static inline unsigned int _guild_count = 0;
	static inline GuildPlayer** _guilds = new GuildPlayer*[GUILDS_DELTA];
	
	/* Add a new guild to the array
	 * return: a new guild object */
	static GuildPlayer* Add(const dpp::snowflake &guild_id);
};

#endif
