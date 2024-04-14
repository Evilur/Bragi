#ifndef BRAGI_MASTER_H
#define BRAGI_MASTER_H

#include <dpp/dpp.h>

inline dpp::discord_client* ds_client;

void on_message_create(dpp::cluster &bot, const dpp::message_create_t &event);
void on_slashcommand(dpp::cluster &bot, const dpp::slashcommand_t &event);
void on_voice_ready(dpp::cluster &bot, const dpp::voice_ready_t &event);
void on_ready(dpp::cluster &bot, const dpp::ready_t &event);

#endif
