#include "bragi.h"
#include "locale/locale.h"
#include "exception/bragi_exception.h"
#include "util/color.h"
#include "client/deezer_client.h"
#include "bragi_hash_map.h"
#include "exception/invalid_arl_exception.h"
#include "util/logger.hpp"
#include "types/string.hpp"

dpp::message Bragi::JoinCommand(const dpp::slashcommand_t& event) {
    /* Default user for connection */
    dpp::snowflake user_id = event.command.usr.id;

    /* Get the user from the command parameter (if exists) */
    if (const dpp::command_value user_parameter = event.get_parameter("user");
        user_parameter.index() != 0)
        user_id = event.command.get_resolved_user(
            std::get<dpp::snowflake>(user_parameter)
        ).id;

    /* Join the channel and return the message */
    return { Join(event, user_id) };
}

dpp::message Bragi::LeaveCommand(const dpp::slashcommand_t& event) {
    /* If the playlist isn't empty, abort the first track to avoid sending
     * the data to the old voice client */
    if (!_playlist.IsEmpty()) AbortPlaying();

    /* Clear the playlist */
    _playlist = Playlist();

    /* Disconnect the voice connection */
    event.from()->disconnect_voice(event.command.guild_id);
    _player.voice_client = nullptr;

    /* Return the result */
    return {_("**:person_walking: Bot has left the voice channel**")};
}

dpp::message Bragi::ListCommand() const {
    /* If the playlist is empty */
    if (_playlist.IsEmpty())
        return dpp::embed()
               .set_color(Color::GREEN)
               .set_title(_("**Playlist is empty**"));

    /* Assemble the message */
    std::stringstream playlist_stream;
    unsigned short counter = 1;
    for (const Track* const track : _playlist) {
        playlist_stream << counter++ << ". " << track->GetTrackData() << '\n';
    }

    /* Return the result */
    return dpp::embed()
           .set_color(Color::GREEN)
           .set_title(_("**Current playlist:**"))
           .set_description(playlist_stream.str());
}

dpp::message Bragi::LoopCommand(const dpp::slashcommand_t& event) {
    /* If there is no parameters, get the next variant in cycle */
    if (const dpp::command_value loop_type_param = event.get_parameter("type");
        loop_type_param.index() == 0)
        _loop_type = LoopType((_loop_type + 1) % 3);
    /* Else get the parameter value */
    else {
        const std::string loop_type_string =
            std::get<std::string>(loop_type_param);
        _loop_type = loop_type_string == "t" ? TRACK
                   : loop_type_string == "p" ? PLAYLIST
                   : DISABLED;
    }

    /* Return the result */
    if (_loop_type == TRACK)
        return {_("**:repeat_one: Track repeat enabled**")};
    if (_loop_type == PLAYLIST)
        return {_("**:repeat: Playlist repeat enabled**")};
    else
        return {_("**:arrow_right: Repeat disabled**")};
}

dpp::message Bragi::NextCommand(const dpp::slashcommand_t& event) {
    /* Get the index of track to replace */
    constexpr long last_track = -1;
    long track_index = last_track;
    if (const dpp::command_value track_index_parameter =
            event.get_parameter("number");
        track_index_parameter.index() != 0)
        track_index = std::get<long>(track_index_parameter) - 1;

    /* If the playlist is empty, throw an exception */
    if (_playlist.IsEmpty())
        throw BragiException(_("**Playlist is empty**"),
                             BragiException::MINOR);

    /* Get the track to replace */
    Track*& track = track_index == last_track ? _playlist.Tail()
                                              : _playlist[track_index];

    /* Get the next track */
    Track* const next_track = track->Next();

    /* If there is no new track */
    if (next_track == nullptr)
        throw BragiException(
            _("**No more tracks have been found for this search query**"),
            BragiException::MINOR
        );

    /* If we are replacing the current playing track, abort it */
    const bool replace_current = track == _playlist.Head();
    if (replace_current) AbortPlaying();

    /* Delete the old track and replace with the new one */
    delete track;
    track = next_track;

    /* If we have replaced the playing track, play the new one */
    if (replace_current) Play();

    /* Return the message */
    return next_track->GetMessage(replace_current);
}

dpp::message Bragi::PlayCommand(const dpp::slashcommand_t& event) {
    /* Get query from the command parameter */
    const std::string query =
        std::get<std::string>(event.get_parameter("query"));

    /* Try to search the Deezer track */
    Track *track = nullptr;
    try {
        track = DeezerClient::Search(query);
    } catch (const InvalidArlException&) {
        throw BragiException(_("**Invalid Deezer ARL token**"),
                             BragiException::MAJOR);
    }

    /* If we can't find a track */
    if (track == nullptr)
        throw BragiException(
            _("**No results have been found for this query**"),
            BragiException::MINOR
        );

    /* Init the result message variable */
    dpp::message result_message = track->GetMessage(_playlist.IsEmpty());

    /* Declare a variable where we will save
     * whether we need to start playing the track or not */
    bool need_to_play;

    /* If the player is not ready */
    if (!IsPlayerReady()) {
        /* Join to the channel and save the message to the result message */
        result_message
            .content.insert(0, Join(event, event.command.usr.id) + '\n');

        /* We don't need to play, because the Play() method will be called
         * from the OnVoiceReady callback */
        need_to_play = false;
    /* If the player is ready */
    } else {
        /* Update the voice client
         * (old can be disconnected even if it is ready) */
        _player.voice_client = event.from()
                                    ->get_voice(event.command.guild_id)
                                    ->voiceclient;

        /* If the playlist is empty now, we push the first track to it.
         * So we need to start playing */
        need_to_play = _playlist.IsEmpty();
    }

    /* Push the track to the playlist */
    _playlist.Push(track);

    /* Play the current track (if we need) */
    if (need_to_play) Play();

    /* Return the result */
    return result_message;
}

dpp::message Bragi::SkipCommand(const dpp::slashcommand_t& event) {
    /* Get number of tracks for skip (if exists) */
    long num_for_skip = 1;
    if (const dpp::command_value num_for_skip_parameter =
            event.get_parameter("number");
        num_for_skip_parameter.index() != 0)
        num_for_skip = std::get<long>(num_for_skip_parameter);

    /* If the playlist is empty */
    if (_playlist.IsEmpty())
        throw BragiException(_("**Playlist is empty**"),
                             BragiException::MINOR);

    /* If we can't skip that number of tracks */
    if (num_for_skip <= 0)
        throw BragiException(
            _("**The number of tracks must be greater than 0**"),
            BragiException::MINOR
        );

    /* Abort the playing track */
    if (!_playlist.IsEmpty()) AbortPlaying();

    /* Skip delete track/tracks from the playlist */
    const unsigned int skipped_num = _playlist.TryPop(num_for_skip);

    /* If the playlist isn't empty, play the next track */
    if (!_playlist.IsEmpty() && IsPlayerReady()) Play();

    /* Return the result */
    return {
        String::Format(_("**:track_next: Tracks skipped: `%u`**"), skipped_num)
    };
}

dpp::message Bragi::SpeedCommand(const dpp::slashcommand_t& event) {
    /* Get a parameter from the user (if exists) */
    const dpp::command_value playback_rate_param =
        event.get_parameter("percent");

    /* If there is NO parameters, set playback rate to 100 (else use param) */
    long playback_rate = playback_rate_param.index() != 0 ?
                         std::get<signed long>(playback_rate_param) : 100;

    /* Check for overflow */
    if (playback_rate < 25)
        throw BragiException(_("**Minimum speed - 25%**"),
                             BragiException::MAJOR);
    if (playback_rate > 250)
        throw BragiException(_("**Maximum speed - 250%**"),
                             BragiException::MAJOR);

    /* If all is OK */
    _player.playback_rate = playback_rate;

    /* If we are playing the track, replay it again */
    if (!_playlist.IsEmpty()) {
        AbortPlaying();
        Play();
    }

    /* Return a message */
    return {
        std::format(_("**:asterisk: Playback speed: `{}%`**"), playback_rate)
    };
}

dpp::message Bragi::PingCommand(const dpp::slashcommand_t& event) {
    return dpp::embed()
           .set_color(Color::GREEN)
           .set_title(_("Ping"))
           .set_description(
               String::Format(_("%.0fms"), event.owner->rest_ping * 1000)
           );
}

void Bragi::OnVoiceReady(const dpp::voice_ready_t& event) {
    /* Log */
    TRACE_LOG("Bot has been connected to a voice channel");

    /* Update the voice */
    _player.voice_client = event.voice_client;

    /* Keep this connection alive */
    _player.voice_client->keepalive = true;

    /* Set the audio type to store data in the buffer before sending */
    _player.voice_client->set_send_audio_type(
        dpp::discord_voice_client::send_audio_type_t::satype_recorded_audio);

    /* If we need to play the first track, play it */
    if (!_playlist.IsEmpty()) Play();
}

void Bragi::OnVoiceStateUpdate(const dpp::voice_state_update_t& event) {
    DEBUG_LOG("VOICE STATE UPDATE");

    /* If there isn't the bot, exit the function */
    if (event.state.user_id != event.owner->me.id) return;

    /* If the voice client isn't initialized, exit the method */
    if (!_player.voice_client) return;

    /* Log this handler */
    TRACE_LOG("Bot has changed its voice state");

    /* If the playlist isn't empty,abort the first track to
     * avoid sending the data to the old voice client */
    if (!_playlist.IsEmpty()) AbortPlaying();

    /* Delete pointer to the old connection */
    _player.voice_client = nullptr;

    /* If the bot reconnects to the other voice channel,
     * connect to the new voice channel */
    if (event.state.channel_id)
        event.from()->connect_voice(event.state.guild_id,
                                    event.state.channel_id);
}

void Bragi::OnMarker() {
    /* Log this handler */
    TRACE_LOG("Bot has received track marker");

    /* Check the loop type */
    if (_loop_type == TRACK) Play();
    else if (_loop_type == PLAYLIST) {
        /* Loop the playlist */
        _playlist.Loop();

        /* Play the next track */
        Play();
    } else {
        /* Remove the first track in the list */
        _playlist.Pop();

        /* If the playlist isn't empty, play the next track */
        if (!_playlist.IsEmpty())
            Play();
    }
}

String Bragi::Join(const dpp::slashcommand_t &event,
                        const dpp::snowflake &user_id) {
    /* Get the user voice channel */
    const dpp::channel* const user_voice_channel = find_channel(
        find_guild(event.command.guild_id)
        ->voice_members.find(user_id)
        ->second.channel_id
    );

    /* If the user isn't in a voice channel */
    if (!user_voice_channel)
        throw BragiException(_("**The user must be in the voice channel**"),
                             BragiException::MAJOR);

    /* If the user and the bot already in the same channel */
    if (IsPlayerReady()
        && _player.voice_client->channel_id == user_voice_channel->id)
        throw BragiException(
            _("**The bot is already connected to this voice channel**"),
            BragiException::MINOR
        );

    /* If bot can not connect to the channel or speak there */
    const dpp::permission channel_permission =
        user_voice_channel->get_user_permissions(&event.owner->me);
    if (!channel_permission.can(dpp::p_connect)
        || !channel_permission.can(dpp::p_speak))
        throw BragiException(
            _("**Unable to connect to the voice channel.\n"
              "Insufficient rights**"),
            BragiException::MAJOR
        );

    /* If we have a track in our playlist, abort it */
    if (!_playlist.IsEmpty()) AbortPlaying();

    /* Reset the old voice client */
    _player.voice_client = nullptr;

    /* Connect to a new channel */
    event.from()->connect_voice(event.command.guild_id, user_voice_channel->id);

    /* Return the result */
    return String::Format(
        _("**:thumbsup: The bot has joined the channel `%s`**"),
        user_voice_channel->name.c_str()
    );
}

void Bragi::Play() {
    /* Join the old thread */
    if (_play_thread.joinable()) _play_thread.join();

    /* Play the track in the new thread */
    _play_thread = std::thread([this] {
        _playlist.Head()->Play(_player);
    });
}

void Bragi::AbortPlaying() {
    /* Abort the track */
    _playlist.Head()->Abort();

    /* Delete voice client packet queue */
    _player.voice_client->stop_audio();

    /* Wait play thread */
    if (_play_thread.joinable()) _play_thread.join();
}


inline bool Bragi::IsPlayerReady() const {
    return _player.voice_client && _player.voice_client->is_ready();
}

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