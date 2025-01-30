#ifndef BRAGI_GUILD_PLAYER_H
#define BRAGI_GUILD_PLAYER_H

#include "track/track.h"
#include "util/linked_list.h"

#include <dpp/dpp.h>

class GuildPlayer final {
public:
	enum LoopType : byte { DISABLED, TRACK, PLAYLIST };

	const dpp::snowflake guild_id;

	explicit GuildPlayer(const dpp::snowflake &guild_id);

	dpp::message PlayCommand(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track* track);

	dpp::message SkipCommand(const dpp::snowflake &channel_id, unsigned short num_for_skip);

	dpp::message SpeedCommand(const dpp::snowflake &channel_id, const byte speed_percent);

	dpp::message ListCommand(const dpp::snowflake &channel_id);

	dpp::message LoopCommand(const dpp::snowflake &channel_id, const LoopType loop_type = (LoopType)((_loop_type + 1) % 3));

	dpp::message NextCommand(const dpp::snowflake &channel_id, unsigned short track_index);

	std::string Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id);

	std::string Leave(const dpp::snowflake &channel_id);

	void HandleReadyState();

	void HandleMarker();

private:
	LinkedList<Track*> _tracks;
	unsigned short _tracks_size = 0;
	const dpp::voiceconn* _voiceconn = nullptr;
	byte _speed_percent = 100;

	bool IsPlayerReady();

	inline bool IsEmpty() const;

public:
	static GuildPlayer* Get(const dpp::snowflake &guild_id);

private:
	static inline LinkedList<GuildPlayer*> _players;
	static inline LoopType _loop_type = DISABLED;

	static GuildPlayer* Add(const dpp::snowflake &guild_id);
};

#endif