#ifndef BRAGI_GUILD_PLAYER_H
#define BRAGI_GUILD_PLAYER_H

#include "player/track/track.h"
#include "playlist.h"

#include <dpp/dpp.h>

class GuildPlayer final {
public:
	const dpp::snowflake guild_id;

	explicit GuildPlayer(const dpp::snowflake &guild_id);

	dpp::message HandleTrack(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track* track);

	dpp::message Skip(const int num_for_skip);

	dpp::message GetPlaylistMessage(const dpp::snowflake &channel_id);

	std::string Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id);

	dpp::message Leave(const dpp::snowflake &channel_id);

	void HandleReadyState();

	void HandleMarker();

public:
	static GuildPlayer* Get(const dpp::snowflake &guild_id);

private:
	const dpp::voiceconn* _voiceconn;
	Playlist _playlist;
	bool _need_to_play_first_track = false;

	bool IsPlayerReady();

private:
	static inline constexpr unsigned short PLAYERS_DELTA = 4;
	static inline unsigned int _max_players_count = PLAYERS_DELTA;
	static inline unsigned int _players_count = 0;
	static inline GuildPlayer** _players = new GuildPlayer* [PLAYERS_DELTA];

	static GuildPlayer* Add(const dpp::snowflake &guild_id);
};

#endif