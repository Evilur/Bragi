#include <dpp/dpp.h>

#include "master.h"
#include "util/dictionary.h"
#include "command/ping.h"
#include "command/join.h"
#include "command/leave.h"
#include "command/play_attachment.h"
#include "player/guild_player.h"
#include "util/logger.h"
#include "util/settings.h"

int main() {
	Logger::Init();  //Init the logger
	Settings::Init();  //Init the settings
	
	dpp::cluster bot(Settings::GetBotToken(), dpp::i_default_intents | dpp::i_message_content);  //Create bot cluster
	
	/* Create event handlers */
	bot.on_message_create([&bot](const dpp::message_create_t &event) { on_message_create(bot, event); });
	bot.on_slashcommand([&bot](const dpp::slashcommand_t &event) { on_slashcommand(bot, event); });
	bot.on_voice_ready([&bot](const dpp::voice_ready_t &event) { on_voice_ready(bot, event); });
	bot.on_ready([&bot](const dpp::ready_t &event) { on_ready(bot, event); });

	Logger::Info("Starting the bot");
	bot.start(dpp::st_wait);
	return 0;
}

void on_message_create(dpp::cluster &bot, const dpp::message_create_t &event) {
	/* If message doesn't start with prefix exit the method */
	if (!event.msg.content.starts_with(Settings::GetPrefix())) return;
	
	/* Get a command name and arguments */
	const unsigned long space_sep = event.msg.content.find(' ');
	std::string command = event.msg.content.substr(1, space_sep - 1);
	std::string arg = event.msg.content.substr(space_sep + 1);

	/* Check for commands */
	if (command == "ping") Ping::Exec(bot, event);
	else if (command == "j" || command == "join") Join::Exec(bot, event);
	else if (command == "leave") Leave::Exec(event);
	else if (command == "pat" || command == "play-attachment") PlayAttachment::Exec(bot, event);
}

void on_slashcommand(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	/* Get a command name */
	std::string command_name = event.command.get_command_name();

	/* Check for commands */
	if (command_name == "ping") Ping::Exec(bot, event);
	else if (command_name == "join") Join::Exec(bot, event);
	else if (command_name == "leave") Leave::Exec(event);
	else if (command_name == "play-attachment") PlayAttachment::Exec(bot, event);
}

void on_voice_ready(dpp::cluster &bot, const dpp::voice_ready_t &event) {
	GuildPlayer::Get(event.voice_client->server_id)->Reconnect();
}

void on_ready(dpp::cluster &bot, const dpp::ready_t &event) {
	/* Init the discord client */
	ds_client = event.from;
	
	/* Add slash commands */
	if (dpp::run_once<struct register_bot_commands>()) {
		bot.global_command_create(dpp::slashcommand("ping", DIC_SLASH_PING, bot.me.id));
		
		bot.global_command_create(dpp::slashcommand("join", DIC_SLASH_JOIN, bot.me.id).add_option(
				dpp::command_option(dpp::co_user, "user", DIC_SLASH_JOIN_USER, false)));

		bot.global_command_create(dpp::slashcommand("leave", DIC_SLASH_LEAVE, bot.me.id));
		
		bot.global_command_create(dpp::slashcommand("play", DIC_SLASH_PLAY, bot.me.id).add_option(
				dpp::command_option(dpp::co_string, "query", DIC_SLASH_PLAY_QUERY, true)));
		
		bot.global_command_create(dpp::slashcommand("play-attachment", DIC_SLASH_PLAY_ATTACHMENT, bot.me.id).add_option(
				dpp::command_option(dpp::co_attachment, "file", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
				dpp::command_option(dpp::co_attachment, "file1", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
				dpp::command_option(dpp::co_attachment, "file2", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
				dpp::command_option(dpp::co_attachment, "file3", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
				dpp::command_option(dpp::co_attachment, "file4", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
				dpp::command_option(dpp::co_attachment, "file5", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
				dpp::command_option(dpp::co_attachment, "file6", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
				dpp::command_option(dpp::co_attachment, "file7", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
				dpp::command_option(dpp::co_attachment, "file8", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
				dpp::command_option(dpp::co_attachment, "file9", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)));
	}
	
	Logger::Info("Bot is ready");
}