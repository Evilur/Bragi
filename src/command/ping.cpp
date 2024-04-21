#include "ping.h"
#include "master.h"
#include "util/color.h"
#include "util/dictionary.h"

#include <chrono>

void Ping::Exec(const dpp::slashcommand_t &event) {
	event.reply(Exec(event.command.channel_id));
}

void Ping::Exec(const dpp::message_create_t &event) {
	event.send(Exec(event.msg.channel_id));
}

dpp::message Ping::Exec(const dpp::snowflake &channel_id) {
	return dpp::message(channel_id, dpp::embed()
			.set_color(Color::GREEN)
			.set_title(DIC_PING)
			.set_description(std::format(DIC_MS, (unsigned short)(bot->rest_ping * 100))));
}