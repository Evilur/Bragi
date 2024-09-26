#ifndef BRAGI_MASTER_H
#define BRAGI_MASTER_H

#include <dpp/dpp.h>

/* Create a macro that outputs detailed logs to the terminal, only if the assembly is built with the DEBUG flag.
 * In the RELEASE build, these logs will not be displayed in the terminal */
#ifndef NDEBUG
#define DEBUG_LOG(str) Logger::Debug(str);
#else
#define DEBUG_LOG(str)
#endif

inline dpp::discord_client* ds_client = nullptr;
inline dpp::cluster* bot = nullptr;

void on_slashcommand(const dpp::slashcommand_t &event);

void on_message_create(const dpp::message_create_t &event);

void on_voice_ready(const dpp::voice_ready_t &event);

void on_voice_track_marker(const dpp::voice_track_marker_t &event);

void on_ready(const dpp::ready_t &event);

typedef signed char int8;

typedef unsigned char u_int8;

typedef signed short int16;

typedef unsigned short u_int16;

typedef signed int int32;

typedef unsigned int u_int32;

typedef signed long int64;

typedef unsigned long u_int64;

#endif