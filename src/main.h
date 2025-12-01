#ifndef BRAGI_MASTER_H
#define BRAGI_MASTER_H

#include <dpp/dpp.h>

void on_slashcommand(const dpp::slashcommand_t &event);

void on_voice_state_update(const dpp::voice_state_update_t &event);

void on_voice_ready(const dpp::voice_ready_t &event);

void on_voice_track_marker(const dpp::voice_track_marker_t &event);

void on_ready(const dpp::ready_t &event);

#endif
