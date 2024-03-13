#include "ping.h"
#include <chrono>
#include <util/color.h>
#include <util/dictionary.h>

void Ping::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	event.reply(dpp::message(event.command.channel_id, Embed(bot)));
}

void Ping::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	event.send(dpp::message(event.msg.channel_id, Embed(bot)));
}

dpp::embed Ping::Embed(dpp::cluster &bot) {
	const auto ping = (unsigned short)(bot.rest_ping / 2 * 1000);
	return dpp::embed()
			.set_color(Color::GREEN)
			.set_title(std::format("**{}**", WORD_PING))
			.set_description(std::format("**{} {}**", ping, WORD_MS));
}