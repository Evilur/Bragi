#include <dpp/dpp.h>

int main() {
	dpp::cluster bot("ODg4MjkyMzE4NjczNzgwNzY3.GXS_sU.UgKKor15p0427r9YLXAze7vxpmnwKcZtz2zXGU");

	bot.on_slashcommand([](auto event) {
		if (event.command.get_command_name() == "ping") {
			event.reply("Fuck the pong!");
		}
	});

	bot.on_ready([&bot](auto event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			bot.global_command_create(
					dpp::slashcommand("ping", "Ping pong!", bot.me.id)
			);
		}
	});

	bot.start(dpp::st_wait);
	return 0;
}