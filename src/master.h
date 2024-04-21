#ifndef BRAGI_MASTER_H
#define BRAGI_MASTER_H

#include <dpp/dpp.h>

inline dpp::discord_client* ds_client;
inline dpp::cluster* bot;

void on_slashcommand(const dpp::slashcommand_t &event);
void on_message_create(const dpp::message_create_t &event);
void on_voice_ready(const dpp::voice_ready_t &event);
void on_ready(const dpp::ready_t &event);

#endif
