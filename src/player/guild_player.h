#ifndef BRAGI_GUILD_PLAYER_H
#define BRAGI_GUILD_PLAYER_H

#include "player/track/track.h"
#include "playlist.h"

#include <dpp/dpp.h>

class GuildPlayer final {
public:
	enum LoopType : u_int8 { DISABLED, TRACK, PLAYLIST };

	const dpp::snowflake guild_id;

	explicit GuildPlayer(const dpp::snowflake &guild_id);

	dpp::message HandleTrack(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track* track);

	dpp::message Skip(const dpp::snowflake &channel_id, const u_int16 num_for_skip);

	dpp::message PlaylistMessage(const dpp::snowflake &channel_id);

	dpp::message Loop(const dpp::snowflake &channel_id, const LoopType loop_type = (LoopType)((_loop_type + 1) % 3));

	dpp::message Next(const dpp::snowflake &channel_id, const u_int16 track_ordinal);

	dpp::message Leave(const dpp::snowflake &channel_id);

	std::string Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id);

	void HandleReadyState();

	void HandleMarker();

public:
	static GuildPlayer* Get(const dpp::snowflake &guild_id);

private:
	const dpp::voiceconn* _voiceconn = nullptr;
	Playlist _playlist;
	bool _need_to_play_first_track = false;

	bool IsPlayerReady();

private:
	static inline constexpr unsigned short PLAYERS_DELTA = 4;
	static inline unsigned int _max_players_count = PLAYERS_DELTA;
	static inline unsigned int _players_count = 0;
	static inline GuildPlayer** _players = new GuildPlayer* [PLAYERS_DELTA];
	static inline LoopType _loop_type = DISABLED;

	static GuildPlayer* Add(const dpp::snowflake &guild_id);
};

#endif