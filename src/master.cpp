#include "master.h"
#include "util/dictionary.h"
#include "command/ping.h"
#include "command/join.h"
#include "command/leave.h"
#include "command/play_attachment.h"
#include "player/guild_player.h"
#include "util/logger.h"
#include "util/settings.h"
#include "client/deezer_client.h"

int main() {
	/* Init static classes */
	Logger::Init();
	Settings::Init();
	DeezerClient::Init();

	DeezerClient::Search("Korpiklaani Vodka");
	return 0;

	/* Create a bot cluster */
	bot = new dpp::cluster(Settings::GetBotToken(), dpp::i_default_intents | dpp::i_message_content);

	/* Create event handlers */
	bot->on_slashcommand([](const dpp::slashcommand_t &event) { on_slashcommand(event); });
	bot->on_message_create([](const dpp::message_create_t &event) { on_message_create(event); });
	bot->on_voice_ready([](const dpp::voice_ready_t &event) { on_voice_ready(event); });
	bot->on_voice_track_marker([](const dpp::voice_track_marker_t &event) { on_voice_track_marker(event); });
	bot->on_ready([](const dpp::ready_t &event) { on_ready(event); });

	/* Start the bot */
	Logger::Info("Starting the bot");
	bot->start(dpp::st_wait);
	return 0;
}

void on_slashcommand(const dpp::slashcommand_t &event) {
	/* Get a command name */
	std::string command_name = event.command.get_command_name();

	/* Check for commands */
	if (command_name == "ping") Ping::Exec(event);
	else if (command_name == "join") Join::Exec(event);
	else if (command_name == "leave") Leave::Exec(event);
	else if (command_name == "play-attachment") PlayAttachment::Exec(event);
}

void on_message_create(const dpp::message_create_t &event) {
	/* If a message doesn't start with the command prefix exit the method */
	if (!event.msg.content.starts_with(Settings::GetPrefix())) return;

	/* Get a command name and arguments */
	const unsigned long space_sep = event.msg.content.find(' ');
	std::string command = event.msg.content.substr(1, space_sep - 1);
	std::string arg = event.msg.content.substr(space_sep + 1);

	/* Check for commands */
	if (command == "ping") Ping::Exec(event);
	else if (command == "j" || command == "join") Join::Exec(event);
	else if (command == "leave") Leave::Exec(event);
	else if (command == "pat" || command == "play-attachment") PlayAttachment::Exec(event);
}

void on_voice_ready(const dpp::voice_ready_t &event) {
	GuildPlayer::Get(event.voice_client->server_id)->HandleReadyState();
}

void on_voice_track_marker(const dpp::voice_track_marker_t &event) {
	GuildPlayer::Get(event.voice_client->server_id)->HandleMarker(event.track_meta);
}

void on_ready(const dpp::ready_t &event) {
	/* Init the discord client */
	ds_client = event.from;

	/* Add slash commands */
	bot->global_command_create(dpp::slashcommand("ping", DIC_SLASH_PING, bot->me.id));

	bot->global_command_create(
			dpp::slashcommand("join", DIC_SLASH_JOIN, bot->me.id).add_option(dpp::command_option(dpp::co_user, "user", DIC_SLASH_JOIN_USER, false)));

	bot->global_command_create(dpp::slashcommand("leave", DIC_SLASH_LEAVE, bot->me.id));

	bot->global_command_create(
			dpp::slashcommand("play", DIC_SLASH_PLAY, bot->me.id).add_option(dpp::command_option(dpp::co_string, "query", DIC_SLASH_PLAY_QUERY, true)));

	bot->global_command_create(dpp::slashcommand("play-attachment", DIC_SLASH_PLAY_ATTACHMENT, bot->me.id).add_option(
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

	Logger::Info("Bot is ready");
}