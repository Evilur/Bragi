#ifndef BRAGI_MASTER_H
#define BRAGI_MASTER_H

#include <dpp/dpp.h>

inline dpp::discord_client* ds_client;
inline dpp::cluster* bot;

void on_slashcommand(const dpp::slashcommand_t &event);

void on_message_create(const dpp::message_create_t &event);

void on_voice_ready(const dpp::voice_ready_t &event);

void on_voice_track_marker(const dpp::voice_track_marker_t &event);

void on_ready(const dpp::ready_t &event);

typedef char int8(signed char);

typedef char uint8(unsigned char);

typedef char int16(signed short);

typedef char uint16(unsigned short);

typedef char int32(signed int);

typedef char uint32(unsigned int);

typedef char int64(signed long);

typedef char uint64(unsigned long);

#endif