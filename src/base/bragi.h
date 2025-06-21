#pragma once

#include "track/track.h"
#include "util/linked_list.h"
#include "util/color.h"
#include "locale/locale.h"

#include <dpp/dpp.h>

class Bragi final {
public:
	enum LoopType : byte { DISABLED, TRACK, PLAYLIST };

    dpp::message JoinCommand(const dpp::slashcommand_t &event);
    dpp::message LeaveCommand(const dpp::slashcommand_t &event);
    dpp::message ListCommand(const dpp::slashcommand_t &event);
    dpp::message LoopCommand(const dpp::slashcommand_t &event);
    dpp::message NextCommand(const dpp::slashcommand_t &event);
    dpp::message PlayCommand(const dpp::slashcommand_t &event);
    dpp::message SkipCommand(const dpp::slashcommand_t &event);
    dpp::message SpeedCommand(const dpp::slashcommand_t &event);

    static dpp::message PingCommand(const dpp::slashcommand_t &event);

    const dpp::snowflake guild_id;

	explicit Bragi(const dpp::snowflake &guild_id);

	dpp::message PlayCommand(const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track* track);

	dpp::message SkipCommand(const dpp::snowflake &channel_id, unsigned short num_for_skip);

	dpp::message SpeedCommand(const dpp::snowflake &channel_id, const byte speed_percent);

	dpp::message ListCommand(const dpp::snowflake &channel_id);

	dpp::message LoopCommand(const dpp::snowflake &channel_id, const LoopType loop_type = (LoopType)((_loop_type + 1) % 3));

	dpp::message NextCommand(const dpp::snowflake &channel_id, unsigned short track_index);

	std::string Join(const dpp::snowflake &user_id, const dpp::snowflake &channel_id);

	std::string Leave(const dpp::snowflake &channel_id);

	void HandleVoiceStateUpdate(const dpp::snowflake &channel_id);

	void HandleReadyState(dpp::discord_voice_client* const voiceconn);

	void HandleMarker();

private:
	LinkedList<Track*> _tracks;
	unsigned short _tracks_size = 0;
	dpp::discord_voice_client* _voiceclient = nullptr;
	byte _playback_rate = 100;

	inline bool IsPlayerReady();

	inline bool IsEmpty() const;

public:
	static Bragi* Get(const dpp::snowflake &guild_id);

private:
	static inline LinkedList<Bragi*> _players;
	static inline LoopType _loop_type = DISABLED;

	static Bragi* Add(const dpp::snowflake &guild_id);
};