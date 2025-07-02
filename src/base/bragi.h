#pragma once

#include "template/linked_list.hpp"

#include <dpp/dpp.h>

class BragiHashMap;
class Track;

class Bragi final {
public:
    class Player {
    public:
        dpp::discord_voice_client *voice_client = nullptr;
        unsigned char playback_rate = 100;
    };

    dpp::message JoinCommand(const dpp::slashcommand_t& event);

    dpp::message LeaveCommand(const dpp::slashcommand_t& event);

    dpp::message ListCommand() const;

    dpp::message LoopCommand(const dpp::slashcommand_t& event);

    dpp::message NextCommand(const dpp::slashcommand_t& event);

    dpp::message PlayCommand(const dpp::slashcommand_t& event);

    dpp::message SkipCommand(const dpp::slashcommand_t& event);

    dpp::message SpeedCommand(const dpp::slashcommand_t& event);

    static dpp::message PingCommand(const dpp::slashcommand_t& event);

    std::string Join(const dpp::slashcommand_t &event,
                     const dpp::snowflake &user_id,
                     const dpp::snowflake &channel_id);

    void OnVoiceReady(const dpp::voice_ready_t& event);

    void OnVoiceStateUpdate(const dpp::voice_state_update_t& event);

    void OnMarker();

private:
    enum LoopType { DISABLED, TRACK, PLAYLIST };

    LinkedList<Track *> _tracks;
    unsigned short _tracks_size = 0;
    LoopType _loop_type = DISABLED;
    Player _player = { nullptr, 100 };
    std::thread _play_thread;

    inline void Play();

    inline void AbortPlaying();

    inline bool IsPlayerReady() const;

    inline bool IsEmpty() const;

public:
    static Bragi* Get(const dpp::snowflake &guild_id);

private:
    static BragiHashMap _bragi_map;
    static constexpr u_int BRAGI_MAP_CAPACITY = 10;
};