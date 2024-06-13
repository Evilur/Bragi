#ifndef BRAGI_PLAY_H
#define BRAGI_PLAY_H

#include <dpp/dpp.h>

class Play final {
public:
	static void Exec(const dpp::slashcommand_t &event);

	static void Exec(const dpp::message_create_t &event);
};

#endif