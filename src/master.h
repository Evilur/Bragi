#ifndef BRAGI_MASTER_H
#define BRAGI_MASTER_H

#include <dpp/dpp.h>

/* Add a var to check whether the build is a Debug or a Relase */
#define DEBUG_BUILD !NDEBUG

inline dpp::discord_client* ds_client;
inline dpp::cluster* bot;

void on_slashcommand(const dpp::slashcommand_t &event);

void on_message_create(const dpp::message_create_t &event);

void on_voice_ready(const dpp::voice_ready_t &event);

void on_voice_track_marker(const dpp::voice_track_marker_t &event);

void on_ready(const dpp::ready_t &event);

typedef signed char int8;

typedef unsigned char uint8;

typedef signed short int16;

typedef unsigned short uint16;

typedef signed int int32;

typedef unsigned int uint32;

typedef signed long int64;

typedef unsigned long uint64;

#endif