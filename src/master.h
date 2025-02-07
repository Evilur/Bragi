#ifndef BRAGI_MASTER_H
#define BRAGI_MASTER_H

#include <dpp/dpp.h>

#define LOG_LEVEL 1
#define DPP_LOG_ENABLED false
#define DPP_LOG_LEVEL 1

typedef unsigned char byte;

typedef signed char s_byte;

inline dpp::discord_client* ds_client = nullptr;
inline dpp::cluster* bot = nullptr;

void on_slashcommand(const dpp::slashcommand_t &event);

void on_message_create(const dpp::message_create_t &event);

void on_voice_state_update(const dpp::voice_state_update_t &event);

void on_voice_ready(const dpp::voice_ready_t &event);

void on_voice_track_marker(const dpp::voice_track_marker_t &event);

void on_ready(const dpp::ready_t &event);

#if DPP_LOG_ENABLED

void on_log(const dpp::log_t &event);

#endif

#endif