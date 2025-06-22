#include "bragi.h"
#include "types/base.h"
#include "master.h"
#include "client/deezer_client.h"
#include "exception/bragi_exception.h"
#include "util/parser.h"

dpp::message Bragi::JoinCommand(const dpp::slashcommand_t &event) {
    /* Default user for connection */
    dpp::snowflake user_id = event.command.usr.id;

    /* Get user from the command parameter (if exists) */
    dpp::command_value user_par = event.get_parameter("user");
    if (user_par.index() != 0) user_id = event.command.get_resolved_user(std::get<dpp::snowflake>(user_par)).id;
}

dpp::message Bragi::LeaveCommand() {
    /* If the playlist isn't empty, abort the first track to avoid sending the data to the old voice client */
    if (!IsEmpty()) _tracks.Head()->Abort();

    /* Disconnect the voice connection */
    ds_client->disconnect_voice(guild_id);
    _voiceclient = nullptr;

    /* Return the result */
    return { _("**:person_walking: Bot has left the voice channel**") };
}

dpp::message Bragi::ListCommand() {
    if (IsEmpty())
        return dpp::embed()
                .set_color(Color::GREEN)
                .set_title(_("**Playlist is empty**"));

    std::stringstream playlist_stream;
    u_int counter = 1;
    for (const Track* const track: _tracks) {
        playlist_stream << counter++ << ". " << track->GetTrackData() << '\n';
    }

    return dpp::embed()
            .set_color(Color::GREEN)
            .set_title(_("**Current playlist:**"))
            .set_description(playlist_stream.str());
}

dpp::message Bragi::LoopCommand(const dpp::slashcommand_t &event) {
    /* Get the loop type slash command parameter */
    dpp::command_value loop_type_param = event.get_parameter("type");

    /* If there is no parameters, get the next variant in cycle */
    if (loop_type_param.index() == 0)
        _loop_type = (LoopType)((_loop_type + 1) % 3);
    /* Else get the parameter value */
    else {
        const std::string loop_type_string =
                std::get<std::string>(loop_type_param);
        _loop_type = loop_type_string == "t" ? TRACK :
                     loop_type_string == "p" ? PLAYLIST :
                     DISABLED;
    }

    /* Return the result */
    if (_loop_type == TRACK)
        return  { _("**:repeat_one: Track repeat enabled**") };
    else if (_loop_type == PLAYLIST)
        return { _("**:repeat: Playlist repeat enabled**") };
    else
        return { _("**:arrow_right: Repeat disabled**") };
}

dpp::message Bragi::NextCommand(const dpp::slashcommand_t &event) { return {"PLACEHOLDER MESSAGE"}; }

dpp::message Bragi::PlayCommand(const dpp::slashcommand_t &event) {
    /* Get query from the command parameter */
    std::string query = std::get<std::string>(event.get_parameter("query"));

    /* Search the Deezer track */
    Track* track = DeezerClient::Search(query);

    /* If there is no such track */
    if (track == nullptr) throw BragiException(DIC_ERROR_TRACK_NOT_FIND,
                                               BragiException::SOFT);

    /* If all is OK */
    bool need_to_play_first_track = IsEmpty();  //If the playlist is empty this track will be played right now
    dpp::message result_msg = track->GetMessage(need_to_play_first_track, event.command.channel_id);  //Get track _message

    /* If player is ready */
    if (IsPlayerReady()) {
        /* Add a track to the playlist */
        _tracks.Push(track);
        _tracks_size++;

        if (need_to_play_first_track) track->AsyncPlay(_voiceclient, _playback_rate);  //Play the current track
        return result_msg;  //Return the track _message
    }

    /* If player is not ready */
    result_msg.content.insert(0, Join(event.command.usr.id, event.command.channel_id) + '\n');  //Join to the channel and insert the _message to the result _message

    /* Add a track to the playlist (if we successfully joined the channel) */
    _tracks.Push(track);
    _tracks_size++;
    return result_msg;  //Return a track _message
}

dpp::message Bragi::SkipCommand(const dpp::slashcommand_t &event) {
    /* Get number of tracks for skip (if exists) */
    unsigned short num_for_skip = 1;
    dpp::command_value num_for_skip_par = event.get_parameter("number");
    if (num_for_skip_par.index() != 0) num_for_skip = std::get<long>(num_for_skip_par);

    /* If the playlist is empty */
    if (IsEmpty()) throw BragiException(DIC_SKIP_PLAYLIST_IS_EMPTY, BragiException::SOFT);

    /* If we can't skip that number of tracks */
    if (num_for_skip == 0) throw BragiException(DIC_SKIP_WRONG_NUM_FOR_SKIP, BragiException::SOFT);

    /* Stop the audio and clear the packet queue */
    _tracks.Head()->Abort();
    if (IsPlayerReady()) _voiceclient->stop_audio();

    /* Get the current track number for skip */
    if (num_for_skip > _tracks_size) num_for_skip = _tracks_size;

    /* Skip delete track/tracks from the playlist */
    _tracks.PopFront(num_for_skip, [](Track* track) { delete track; });
    _tracks_size -= num_for_skip;

    /* If the playlist isn't empty, play the next track */
    if (!IsEmpty() && IsPlayerReady()) _tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);

    return {std::format(DIC_SKIP_MSG, num_for_skip)};
}

dpp::message Bragi::SpeedCommand(const dpp::slashcommand_t &event) {
    /* Get a parameter from the user (if exists) */
    dpp::command_value playback_rate_param = event.get_parameter("percent");

    /* If there is NO parameters, set playback rate to 100 (else use param) */
    s_long playback_rate = playback_rate_param.index() != 0 ?
                           std::get<s_long>(playback_rate_param) : 100;

    /* Check for overflow */
    if (playback_rate < 25)
        throw BragiException(_("**Minimum speed - 25%**"),
                             BragiException::HARD);
    else if (playback_rate > 250)
        throw BragiException(_("**Maximum speed - 250%**"),
                             BragiException::HARD);

    /* If all is OK */
    _playback_rate = playback_rate;

    /* Return a _message */
    return {
        std::format(_("**:asterisk: Playback speed: `{}%`**"), playback_rate)
    };
}

dpp::message Bragi::PingCommand() {
    /* Get the ping */
    uint ping = (uint)(bot->rest_ping * 1000.0);

    /* Return the result */
    return dpp::embed()
            .set_color(Color::GREEN)
            .set_title(_("Ping"))
            .set_description(std::format(_("{}ms"), ping));
}
