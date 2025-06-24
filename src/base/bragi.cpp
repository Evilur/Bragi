#include "master.h"
#include "bragi.h"
#include "locale/locale.h"
#include "exception/bragi_exception.h"
#include "util/color.h"
#include "types/base.h"
#include "client/deezer_client.h"
#include "bragi_hash_map.h"
#include "exception/InvalidArlException.h"

dpp::message Bragi::JoinCommand(const dpp::slashcommand_t &event) {
    /* Default user for connection */
    dpp::snowflake user_id = event.command.usr.id;

    /* Get user from the command parameter (if exists) */
    dpp::command_value user_par = event.get_parameter("user");
    if (user_par.index() != 0)
        user_id = event.command.get_resolved_user(
            std::get<dpp::snowflake>(user_par)).id;
}

dpp::message Bragi::LeaveCommand(const dpp::slashcommand_t &event) {
    /* If the playlist isn't empty, abort the first track to avoid sending the data to the old voice client */
    if (!IsEmpty())
        _tracks.Head()->Abort();

    /* Disconnect the voice connection */
    event.from()->disconnect_voice(event.command.guild_id);
    _voiceclient = nullptr;

    /* Return the result */
    return {_("**:person_walking: Bot has left the voice channel**")};
}

dpp::message Bragi::ListCommand() const {
    if (IsEmpty())
        return dpp::embed()
               .set_color(Color::GREEN)
               .set_title(_("**Playlist is empty**"));

    std::stringstream playlist_stream;
    u_int counter = 1;
    for (const Track *const track : _tracks) {
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
        const std::string loop_type_string = std::get<std::string>(
            loop_type_param);
        _loop_type =
            loop_type_string == "t"
                ? TRACK
                : loop_type_string == "p"
                ? PLAYLIST
                : DISABLED;
    }

    /* Return the result */
    if (_loop_type == TRACK)
        return {_("**:repeat_one: Track repeat enabled**")};
    if (_loop_type == PLAYLIST)
        return {_("**:repeat: Playlist repeat enabled**")};
    return {_("**:arrow_right: Repeat disabled**")};
}

dpp::message Bragi::NextCommand(const dpp::slashcommand_t &event) {
    /* Get number of tracks for skip (if exists) */
    s_long track_index = 0;
    if (const dpp::command_value track_num_par = event.get_parameter("number");
        track_num_par.index() != 0)
            track_index = std::get<long>(track_num_par);

    /* If the playlist is empty, throw an exception */
    if (IsEmpty())
        throw BragiException(
            DIC_SLASH_NEXT_PLAYLIST_EMPTY, BragiException::SOFT);

    /* Get the index of the track */
    if (track_index == 0 || track_index > _tracks_size)
        track_index = _tracks_size - 1;
    else
        track_index--;

    /* If we need to replace the current playing track, abort it */
    const bool is_playing = !track_index;
    if (is_playing)
        _tracks.Head()->Abort();

    /* Get the next track pointer */
    Track * &old_track = _tracks[track_index];
    Track *next_track = old_track->Next();

    /* If there is no new track */
    if (next_track == nullptr)
        throw BragiException(DIC_SLASH_NEXT_NO_RESULTS, BragiException::SOFT);

    /* Delete the old track and replace with the next one */
    delete old_track;
    old_track = next_track;

    /* If the old track is playing, stop the audio, clear the packet queue, and play the new track */
    if (is_playing) {
        _voiceclient->stop_audio();
        if (IsPlayerReady())
            next_track->AsyncPlay(_voiceclient, _playback_rate);
    }

    /* Return the _message */
    return next_track->GetMessage(is_playing, event.command.channel_id);
}

dpp::message Bragi::PlayCommand(const dpp::slashcommand_t &event) {
    /* Get query from the command parameter */
    std::string query = std::get<std::string>(event.get_parameter("query"));

    /* Try to search the Deezer track */
    Track *track = nullptr;
    try { track = DeezerClient::Search(query); }
    catch (const InvalidArlException&) {
        throw BragiException(_("**Invalid Deezer ARL token**"), BragiException::HARD);
    }

    /* If there is no such track */
    if (track == nullptr)
        throw BragiException(DIC_ERROR_TRACK_NOT_FIND,
                             BragiException::SOFT);

    /* If all is OK */
    const bool need_to_play_first_track = IsEmpty();
    //If the playlist is empty this track will be played right now
    dpp::message result_msg = track->GetMessage(need_to_play_first_track,
                                                event.command.channel_id);
    //Get track _message

    /* If player is ready */
    if (IsPlayerReady()) {
        /* Add a track to the playlist */
        _tracks.Push(track);
        _tracks_size++;

        if (need_to_play_first_track)
            track->AsyncPlay(_voiceclient, _playback_rate);
        //Play the current track
        return result_msg; //Return the track _message
    }

    /* If player is not ready */
    result_msg.content.insert(
        0, Join(event, event.command.usr.id, event.command.channel_id) + '\n');
    //Join to the channel and insert the _message to the result _message

    /* Add a track to the playlist (if we successfully joined the channel) */
    _tracks.Push(track);
    _tracks_size++;
    return result_msg; //Return a track _message
}

dpp::message Bragi::SkipCommand(const dpp::slashcommand_t &event) {
    /* Get number of tracks for skip (if exists) */
    unsigned short num_for_skip = 1;
    dpp::command_value num_for_skip_par = event.get_parameter("number");
    if (num_for_skip_par.index() != 0)
        num_for_skip = std::get<long>(num_for_skip_par);

    /* If the playlist is empty */
    if (IsEmpty())
        throw BragiException(DIC_SKIP_PLAYLIST_IS_EMPTY, BragiException::SOFT);

    /* If we can't skip that number of tracks */
    if (num_for_skip == 0)
        throw BragiException(DIC_SKIP_WRONG_NUM_FOR_SKIP, BragiException::SOFT);

    /* Stop the audio and clear the packet queue */
    _tracks.Head()->Abort();
    if (IsPlayerReady())
        _voiceclient->stop_audio();

    /* Get the current track number for skip */
    if (num_for_skip > _tracks_size)
        num_for_skip = _tracks_size;

    /* Skip delete track/tracks from the playlist */
    _tracks.PopFront(num_for_skip, [](Track *track) { delete track; });
    _tracks_size -= num_for_skip;

    /* If the playlist isn't empty, play the next track */
    if (!IsEmpty() && IsPlayerReady())
        _tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);

    return {std::format(DIC_SKIP_MSG, num_for_skip)};
}

dpp::message Bragi::SpeedCommand(const dpp::slashcommand_t &event) {
    /* Get a parameter from the user (if exists) */
    dpp::command_value playback_rate_param = event.get_parameter("percent");

    /* If there is NO parameters, set playback rate to 100 (else use param) */
    s_long playback_rate = playback_rate_param.index() != 0
                               ? std::get<s_long>(playback_rate_param)
                               : 100;

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

dpp::message Bragi::PingCommand(const dpp::slashcommand_t& event) {
    /* Get the ping */
    unsigned int ping = (unsigned int)(event.owner->rest_ping * 1000.0);

    /* Return the result */
    return dpp::embed()
           .set_color(Color::GREEN)
           .set_title(_("Ping"))
           .set_description(std::format(_("{}ms"), ping));
}

std::string Bragi::Join(const dpp::slashcommand_t &event,
                        const dpp::snowflake &user_id,
                        const dpp::snowflake &channel_id) {
    /* Get the user voice channel */
    const dpp::guild *const guild = dpp::find_guild(event.command.guild_id);
    const dpp::channel *const user_voice_channel = dpp::find_channel(
        guild->voice_members.find(user_id)->second.channel_id);

    /* If the user isn't in a voice channel */
    if (!user_voice_channel)
        throw BragiException(
            DIC_ERROR_USER_NOT_IN_VOICE_CHANNEL, BragiException::HARD);

    /* If the user and a bot already in the same channel */
    if (IsPlayerReady() && _voiceclient->channel_id == user_voice_channel->id)
        throw BragiException(
            DIC_ERROR_ALREADY_IN_CURRENT_CHANNEL, BragiException::SOFT);

    /* If bot can not connect to the channel or speak there */
    dpp::permission channel_permission = user_voice_channel->
        get_user_permissions(&event.owner->me);
    if (!channel_permission.can(dpp::p_connect) || !channel_permission.can(
            dpp::p_speak))
        throw BragiException(DIC_ERROR_PERMISSION_DENIED, BragiException::HARD);

    /* If we have a track in our playlist, abort it */
    if (!IsEmpty())
        _tracks.Head()->Abort();

    /* Connect to the new channel */
    event.from()->connect_voice(event.command.guild_id, user_voice_channel->id);

    /* Reset the old voice connection */
    _voiceclient = nullptr;

    /* Return the result to the channel */
    return std::format(DIC_JOINED, user_voice_channel->name);
}

void Bragi::HandleMarker() {
    /* Check the loop type */
    if (_loop_type == TRACK)
        _tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);
    else if (_loop_type == PLAYLIST) {
        /* Move the first track to the end of the playlist */
        Track *track = _tracks.Head();
        _tracks.PopFront();
        _tracks.Push(track);

        /* Play the next track */
        _tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);
    } else {
        /* Remove the first track in the list */
        _tracks.PopFront([](Track *track) { delete track; });
        _tracks_size--;

        /* If the playlist isn't empty, play the next track */
        if (!IsEmpty())
            _tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);
    }
}

void Bragi::HandleVoiceStateUpdate(const dpp::voice_state_update_t &event,
                                   const dpp::snowflake &channel_id) {
    /* If the voice client isn't initialized, exit the method */
    if (!_voiceclient)
        return;

    /* If the voice channel doesn't change, exit the method */
    if (_voiceclient->channel_id == channel_id)
        return;

    /* If the playlist isn't empty, abort the first track to avoid sending the data to the old voice client */
    if (!IsEmpty())
        _tracks.Head()->Abort();

    /* Reset the old voice connection */
    _voiceclient->stop_audio();
    _voiceclient = nullptr;

    /* If the bot reconnect to the other voice channelm connect to the new voice channel */
    if (channel_id)
        event.from()->connect_voice(event.state.guild_id, channel_id);
}

void Bragi::HandleReadyState(dpp::discord_voice_client *const voiceclient) {
    /* Update the voice */
    _voiceclient = voiceclient;

    /* Set the audio type to store data in the buffer before sending */
    _voiceclient->set_send_audio_type(
        dpp::discord_voice_client::send_audio_type_t::satype_recorded_audio);

    /* If we need to play the first track, play it */
    if (!IsEmpty())
        _tracks.Head()->AsyncPlay(_voiceclient, _playback_rate);
}

inline bool Bragi::IsPlayerReady() {
    return _voiceclient && _voiceclient->is_ready();
}

inline bool Bragi::IsEmpty() const { return !_tracks_size; }

#pragma region Static Members

BragiHashMap Bragi::_bragi_map = BragiHashMap(BRAGI_MAP_CAPACITY);

Bragi *Bragi::Get(const dpp::snowflake &guild_id) {
    /* Try to get the guild in the hash map */
    Bragi *bragi = _bragi_map.Get(guild_id);

    /* If all is OK, return it */
    if (bragi) return bragi;

    /* If there is not such an instance, create a new one */
    bragi = new Bragi();

    /* Add it to the hash map */
    _bragi_map.Put(guild_id, bragi);

    /* Return a new instance */
    return bragi;
}

#pragma endregion