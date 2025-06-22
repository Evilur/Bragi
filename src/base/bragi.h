#pragma once

#include "track/track.h"
#include "util/linked_list.h"

#include <dpp/dpp.h>

class BragiHashMap;

class Bragi final {
public:
    dpp::message JoinCommand(const dpp::slashcommand_t &event);

    dpp::message LeaveCommand();

    dpp::message ListCommand();

    dpp::message LoopCommand(const dpp::slashcommand_t &event);

    dpp::message NextCommand(const dpp::slashcommand_t &event);

    dpp::message PlayCommand(const dpp::slashcommand_t &event);

    dpp::message SkipCommand(const dpp::slashcommand_t &event);

    dpp::message SpeedCommand(const dpp::slashcommand_t &event);

    static dpp::message PingCommand();

    const dpp::snowflake guild_id;

	explicit Bragi(const dpp::snowflake &guild_id);

	dpp::message NextCommand(const dpp::snowflake &channel_id, unsigned short track_index);

	std::string Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id);

	void HandleVoiceStateUpdate(const dpp::snowflake &channel_id);

	void HandleReadyState(dpp::discord_voice_client* const voiceconn);

	void HandleMarker();

private:
    enum LoopType { DISABLED, TRACK, PLAYLIST };

    LinkedList<Track*> _tracks;
	unsigned short _tracks_size = 0;
	dpp::discord_voice_client* _voiceclient = nullptr;
    LoopType _loop_type = DISABLED;
	u_byte _playback_rate = 100;

	inline bool IsPlayerReady();

	inline bool IsEmpty() const;

public:
	static Bragi* Get(const dpp::snowflake &guild_id);

private:
	static BragiHashMap _bragi_map;
	constexpr u_int _bragi_map_size = 10;
};
