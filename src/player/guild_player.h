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

	void HandleReadyState();

	void HandleMarker(const std::string &meta);

public:
	static GuildPlayer* Get(const dpp::snowflake &guild_id);

private:
	dpp::voiceconn* voiceconn_;
	Playlist playlist_;

	void SendOpus(Track* track);

	bool IsPlayerReady();

private:
	static inline constexpr unsigned short PLAYERS_DELTA = 4;
	static inline unsigned int max_players_count_ = PLAYERS_DELTA;
	static inline unsigned int players_count_ = 0;
	static inline GuildPlayer** players_ = new GuildPlayer* [PLAYERS_DELTA];

	static GuildPlayer* Add(const dpp::snowflake &guild_id);
};

#endif
