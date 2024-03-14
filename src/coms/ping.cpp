#include "ping.h"
#include <chrono>
#include <util/color.h>
#include <util/dictionary.h>

void Ping::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	event.reply(Message(bot, event.command.channel_id));
}

void Ping::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	event.send(Message(bot, event.msg.channel_id));
}

dpp::message Ping::Message(dpp::cluster &bot, dpp::snowflake channel_id) {
	return dpp::message(channel_id, dpp::embed()
			.set_color(Color::GREEN)
			.set_title(DIC_PING)
			.set_description(std::format(DIC_MS, (unsigned short)(bot.rest_ping * 100))));
}