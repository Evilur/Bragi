#ifndef BRAGI_LOOP_H
#define BRAGI_LOOP_H

#include "master.h"
#include "guild//guild_player.h"

#include <dpp/dpp.h>

class Loop final {
public:
	static void Exec(const dpp::slashcommand_t &event);

	static void Exec(const dpp::message_create_t &event, const std::string &argument);
};

#endif