#include "master.h"
#include "base/bragi.h"
#include "exception/bragi_exception.h"
#include "locale/locale.h"
#include "util/logger.h"
#include "util/properties.h"

int main() {
    /* Create a bot cluster */
    dpp::cluster bot = dpp::cluster(Properties::BotToken());

    /* Create event handlers */
    bot.on_slashcommand(on_slashcommand);
    bot.on_voice_state_update(on_voice_state_update);
    bot.on_voice_ready(on_voice_ready);
    bot.on_voice_track_marker(on_voice_track_marker);
    bot.on_ready(on_ready);

    /* Start the bot */
    INFO_LOG("Starting the bot");
    bot.start(dpp::st_wait);
}

void on_slashcommand(const dpp::slashcommand_t &event) {
    /* Get a command name */
    const std::string command_name = event.command.get_command_name();

    /* Get the bragi instance */
    Bragi *const bragi = Bragi::Get(event.command.guild_id);

    try {
        /* Run a command and get a result _message */
        const dpp::message message =
            command_name == "play"
                ? bragi->PlayCommand(event)
                : command_name == "skip"
                ? bragi->SkipCommand(event)
                : command_name == "list"
                ? bragi->ListCommand()
                : command_name == "next"
                ? bragi->NextCommand(event)
                : command_name == "loop"
                ? bragi->LoopCommand(event)
                : command_name == "join"
                ? bragi->JoinCommand(event)
                : command_name == "speed"
                ? bragi->SpeedCommand(event)
                : command_name == "leave"
                ? bragi->LeaveCommand(event)
                : command_name == "ping"
                ? Bragi::PingCommand(event)
                : throw BragiException(_("**Unexpected error**"),
                                       BragiException::HARD);
        event.reply(message);
    } catch (const BragiException &e) {
        /* Handle the exception and print the error message to the user */
        const dpp::message &message = e.GetMessage();
        event.reply(message);
    }
}

void on_voice_state_update(const dpp::voice_state_update_t &event) {
    /* If there isn't the bot, exit the function */
    if (event.state.user_id != event.owner->me.id)
        return;

    /* Handle the voice state update */
    Bragi::Get(event.state.guild_id)->OnVoiceStateUpdate(event);
}

void on_voice_ready(const dpp::voice_ready_t &event) {
    Bragi::Get(event.voice_client->server_id)->OnVoiceReady(event);
}

void on_voice_track_marker(const dpp::voice_track_marker_t &event) {
    Bragi::Get(event.voice_client->server_id)->OnMarker();
}

void on_ready(const dpp::ready_t &event) {
    dpp::cluster *bot = event.owner;

    /* Add slash commands */
    bot->global_command_create(
        dpp::slashcommand("ping", DIC_SLASH_PING, bot->me.id));

    bot->global_command_create(
        dpp::slashcommand("join", DIC_SLASH_JOIN, bot->me.id).add_option(
            dpp::command_option(dpp::co_user, "user", DIC_SLASH_JOIN_USER,
                                false)));

    bot->global_command_create(
        dpp::slashcommand("leave", DIC_SLASH_LEAVE, bot->me.id));

    bot->global_command_create(
        dpp::slashcommand("play", DIC_SLASH_PLAY, bot->me.id).add_option(
            dpp::command_option(dpp::co_string, "query", DIC_SLASH_PLAY_QUERY,
                                true)));

    bot->global_command_create(
        dpp::slashcommand("skip", DIC_SLASH_SKIP, bot->me.id).add_option(
            dpp::command_option(dpp::co_integer, "number",
                                DIC_SLASH_SKIP_NUMBER, false)));

    bot->global_command_create(
        dpp::slashcommand("speed", DIC_SLASH_SPEED, bot->me.id).add_option(
            dpp::command_option(dpp::co_integer, "percent",
                                DIC_SLASH_SPEED_PERCENT, false)));

    bot->global_command_create(
        dpp::slashcommand("next", DIC_SLASH_NEXT, bot->me.id).add_option(
            dpp::command_option(dpp::co_integer, "number",
                                DIC_SLASH_NEXT_ORDINAL, false)));

    bot->global_command_create(
        dpp::slashcommand("loop", DIC_SLASH_LOOP, bot->me.id).add_option(
            dpp::command_option(dpp::co_string, "type", DIC_SLASH_LOOP_TYPE,
                                false).add_choice(
                dpp::command_option_choice("Disabled", "d")).add_choice(
                dpp::command_option_choice("Track", "t")).add_choice(
                dpp::command_option_choice("Playlist", "p"))));

    bot->global_command_create(
        dpp::slashcommand("list", DIC_SLASH_LIST, bot->me.id));

    INFO_LOG("Bot is ready");
}