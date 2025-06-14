#include "master.h"
#include "util/dictionary.h"
#include "command/ping.h"
#include "command/join.h"
#include "command/leave.h"
#include "guild/guild_player.h"
#include "util/logger.h"
#include "util/settings.h"
#include "client/deezer_client.h"
#include "command/play.h"
#include "command/list.h"
#include "command/skip.h"
#include "command/loop.h"
#include "command/next.h"
#include "util/path.h"
#include "command/speed.h"

int main() {
	/* Init static classes */
	Path::Init();
	Logger::Init();
	Settings::Init();
	DeezerClient::Init();

	/* Create a bot cluster */
	bot = new dpp::cluster(Settings::GetBotToken(),
                           dpp::i_default_intents | dpp::i_message_content,
                           0, 0, 1, false);

	/* Create event handlers */
	bot->on_slashcommand(on_slashcommand);
	bot->on_message_create(on_message_create);
	bot->on_voice_state_update(on_voice_state_update);
	bot->on_voice_ready(on_voice_ready);
	bot->on_voice_track_marker(on_voice_track_marker);
	bot->on_ready(on_ready);
#if DPP_LOG_ENABLED
	bot->on_log(on_log);
#endif

	/* Start the bot */
	INFO_LOG("Starting the bot");
	bot->start(dpp::st_wait);
	return 0;
}

void on_slashcommand(const dpp::slashcommand_t &event) {
	/* Get a command name */
	std::string command_name = event.command.get_command_name();

	/* Check for commands */
	if (command_name == "play") Play::Exec(event);
	else if (command_name == "skip") Skip::Exec(event);
	else if (command_name == "list") List::Exec(event);
	else if (command_name == "next") Next::Exec(event);
	else if (command_name == "loop") Loop::Exec(event);
	else if (command_name == "ping") Ping::Exec(event);
	else if (command_name == "join") Join::Exec(event);
	else if (command_name == "speed") Speed::Exec(event);
	else if (command_name == "leave") Leave::Exec(event);
}

void on_message_create(const dpp::message_create_t &event) {
	/* If a message doesn't start with the command prefix exit the method */
	if (!event.msg.content.starts_with(Settings::GetPrefix())) return;

	/* Get a command name and arguments */
	const unsigned long space_sep = event.msg.content.find(' ');
	std::string command = event.msg.content.substr(1, space_sep - 1);
	std::string argument = space_sep == std::string::npos ? "" : event.msg.content.substr(space_sep + 1);

	/* Check for commands */
	if (command == "p" || command == "play") Play::Exec(event, argument);
	else if (command == "s" || command == "skip") Skip::Exec(event, argument);
	else if (command == "l" || command == "list") List::Exec(event);
	else if (command == "n" || command == "next") Next::Exec(event, argument);
	else if (command == "loop") Loop::Exec(event, argument);
	else if (command == "ping") Ping::Exec(event);
	else if (command == "j" || command == "join") Join::Exec(event);
	else if (command == "sp" || command == "speed") Speed::Exec(event, argument);
	else if (command == "teave") Leave::Exec(event);
}

void on_voice_state_update(const dpp::voice_state_update_t &event) {
	/* If there isn't the bot, exit the function */
	if (event.state.user_id != bot->me.id) return;

	/* Handle the voice state udpate */
	GuildPlayer::Get(event.state.guild_id)->HandleVoiceStateUpdate(event.state.channel_id);
}

void on_voice_ready(const dpp::voice_ready_t &event) {
	GuildPlayer::Get(event.voice_client->server_id)->HandleReadyState(event.voice_client);
}

void on_voice_track_marker(const dpp::voice_track_marker_t &event) {
	GuildPlayer::Get(event.voice_client->server_id)->HandleMarker();
}

void on_ready(const dpp::ready_t &event) {
	/* Init the discord client */
	ds_client = event.from();

	/* Add slash commands */
	bot->global_command_create(dpp::slashcommand("ping", DIC_SLASH_PING, bot->me.id));

	bot->global_command_create(
			dpp::slashcommand("join", DIC_SLASH_JOIN, bot->me.id).add_option(dpp::command_option(dpp::co_user, "user", DIC_SLASH_JOIN_USER, false)));

	bot->global_command_create(dpp::slashcommand("leave", DIC_SLASH_LEAVE, bot->me.id));

	bot->global_command_create(
			dpp::slashcommand("play", DIC_SLASH_PLAY, bot->me.id).add_option(dpp::command_option(dpp::co_string, "query", DIC_SLASH_PLAY_QUERY, true)));

	bot->global_command_create(
			dpp::slashcommand("skip", DIC_SLASH_SKIP, bot->me.id)
					.add_option(dpp::command_option(dpp::co_integer, "number", DIC_SLASH_SKIP_NUMBER, false)));

	bot->global_command_create(
			dpp::slashcommand("speed", DIC_SLASH_SPEED, bot->me.id)
					.add_option(dpp::command_option(dpp::co_integer, "percent", DIC_SLASH_SPEED_PERCENT, false)));

	bot->global_command_create(
			dpp::slashcommand("next", DIC_SLASH_NEXT, bot->me.id).add_option(dpp::command_option(dpp::co_integer, "number", DIC_SLASH_NEXT_ORDINAL, false)));

	bot->global_command_create(dpp::slashcommand("loop", DIC_SLASH_LOOP, bot->me.id).add_option(
			dpp::command_option(dpp::co_string, "type", DIC_SLASH_LOOP_TYPE, false)
					.add_choice(dpp::command_option_choice("Disabled", "d"))
					.add_choice(dpp::command_option_choice("Track", "t"))
					.add_choice(dpp::command_option_choice("Playlist", "p"))));

	bot->global_command_create(dpp::slashcommand("list", DIC_SLASH_LIST, bot->me.id));

	/*bot->global_command_create(dpp::slashcommand("play-attachment", DIC_SLASH_PLAY_ATTACHMENT, bot->me.id).add_option(
			dpp::command_option(dpp::co_attachment, "file", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
			dpp::command_option(dpp::co_attachment, "file1", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
			dpp::command_option(dpp::co_attachment, "file2", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
			dpp::command_option(dpp::co_attachment, "file3", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
			dpp::command_option(dpp::co_attachment, "file4", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
			dpp::command_option(dpp::co_attachment, "file5", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
			dpp::command_option(dpp::co_attachment, "file6", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
			dpp::command_option(dpp::co_attachment, "file7", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
			dpp::command_option(dpp::co_attachment, "file8", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)).add_option(
			dpp::command_option(dpp::co_attachment, "file9", DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT, false)));*/

	INFO_LOG("Bot is ready");
}

#if DPP_LOG_ENABLED

void on_log(const dpp::log_t &event) {
	/* Check the log level */
	if (event.severity < DPP_LOG_LEVEL) return;

	/* Log the message */
	Logger::Log(event.message, (Logger::LogLevel)event.severity);
}

#endif
