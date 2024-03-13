#ifndef BRAGI_PING_H
#define BRAGI_PING_H

#include "cluster.h"
#include "dispatcher.h"

class Ping {
public:
	static void Exec(dpp::cluster &bot, const dpp::slashcommand_t &event);
	static void Exec(dpp::cluster &bot, const dpp::message_create_t &event);
private:
	static dpp::embed Embed(dpp::cluster &bot);
};


#endif
