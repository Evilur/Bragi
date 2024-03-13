#include <dpp/dpp.h>
#include <coms/ping.h>
#include <util/logger.h>
#include <util/settings.h>

void on_message_create(dpp::cluster &bot, const dpp::message_create_t &event);
void on_slashcommand(dpp::cluster &bot, const dpp::slashcommand_t &event);
void on_ready(dpp::cluster &bot, const dpp::ready_t &event);

int main() {
	Logger::Init();  //Init the logger
	Settings::Init();  //Init the settings
	
	dpp::cluster bot(Settings::GetBotToken(), dpp::i_default_intents | dpp::i_message_content);  //Create bot cluster
	
	/* Create event handlers */
	bot.on_message_create([&bot](const dpp::message_create_t &event) { on_message_create(bot, event); });
	bot.on_slashcommand([&bot](const dpp::slashcommand_t &event) { on_slashcommand(bot, event); });
	bot.on_ready([&bot](const dpp::ready_t &event) { on_ready(bot, event); });

	Logger::Info("Starting the bot");
	bot.start(dpp::st_wait);
	return 0;
}

void on_message_create(dpp::cluster &bot, const dpp::message_create_t &event) {
	/* If message doesn't start with prefix exit the method */
	if (!event.msg.content.starts_with(Settings::GetPrefix())) return;
	
	const unsigned long space_sep = event.msg.content.find(' ');
	std::string command = event.msg.content.substr(1, space_sep - 1);
	std::string arg = event.msg.content.substr(space_sep + 1);
	
	if (command == "ping") Ping::Exec(bot, event);
}

void on_slashcommand(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	if (event.command.get_command_name() == "ping") Ping::Exec(bot, event);
}

void on_ready(dpp::cluster &bot, const dpp::ready_t &event) {
	if (dpp::run_once<struct register_bot_commands>()) {
		bot.global_command_create(dpp::slashcommand("ping", "Get ping", bot.me.id));
	}
	
	Logger::Info("Bot is ready");
}