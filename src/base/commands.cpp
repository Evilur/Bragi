#include "bragi.h"
#include "client/deezer_client.h"
#include "exception/bragi_exception.h"

void Bragi::JoinCommand(const dpp::slashcommand_t &event) { }
void Bragi::LeaveCommand(const dpp::slashcommand_t &event) { }
void Bragi::ListCommand(const dpp::slashcommand_t &event) { }
void Bragi::LoopCommand(const dpp::slashcommand_t &event) { }
void Bragi::NextCommand(const dpp::slashcommand_t &event) { }
void Bragi::PingCommand(const dpp::slashcommand_t &event) { }

dpp::message Bragi::PlayCommand(const dpp::slashcommand_t &event) {
    /* Get query from the command parameter */
    std::string query = std::get<std::string>(event.get_parameter("query"));

    /* Search the Deezer track */
    Track* track = DeezerClient::Search(query);

    /* If there is no such track */
    if (track == nullptr) throw BragiException(DIC_ERROR_TRACK_NOT_FIND,
                                               event.command.channel_id,
                                               BragiException::SOFT);

    /* If all is OK */
    bool need_to_play_first_track = IsEmpty();  //If the playlist is empty this track will be played right now
    dpp::message result_msg = track->GetMessage(need_to_play_first_track, event.command.channel_id);  //Get track message

    /* If player is ready */
    if (IsPlayerReady()) {
        /* Add a track to the playlist */
        _tracks.Push(track);
        _tracks_size++;

        if (need_to_play_first_track) track->AsyncPlay(_voiceclient, _speed_percent);  //Play the current track
        return result_msg;  //Return the track message
    }

    /* If player is not ready */
    result_msg.content.insert(0, Join(event.command.usr.id, event.command.channel_id) + '\n');  //Join to the channel and insert the message to the result message

    /* Add a track to the playlist (if we successfully joined the channel) */
    _tracks.Push(track);
    _tracks_size++;
    return result_msg;  //Return a track message
}

dpp::message Bragi::SkipCommand(const dpp::slashcommand_t &event) {
    /* Get number of tracks for skip (if exists) */
    unsigned short num_for_skip = 1;
    dpp::command_value num_for_skip_par = event.get_parameter("number");
    if (num_for_skip_par.index() != 0) num_for_skip = std::get<long>(num_for_skip_par);

    /* If the playlist is empty */
    if (IsEmpty()) throw BragiException(DIC_SKIP_PLAYLIST_IS_EMPTY, event.command.channel_id, BragiException::SOFT);

    /* If we can't skip that number of tracks */
    if (num_for_skip == 0) throw BragiException(DIC_SKIP_WRONG_NUM_FOR_SKIP, event.command.channel_id, BragiException::SOFT);

    /* Stop the audio and clear the packet queue */
    _tracks.Head()->Abort();
    if (IsPlayerReady()) _voiceclient->stop_audio();

    /* Get the current track number for skip */
    if (num_for_skip > _tracks_size) num_for_skip = _tracks_size;

    /* Skip delete track/tracks from the playlist */
    _tracks.PopFront(num_for_skip, [](Track* track) { delete track; });
    _tracks_size -= num_for_skip;

    /* If the playlist isn't empty, play the next track */
    if (!IsEmpty() && IsPlayerReady()) _tracks.Head()->AsyncPlay(_voiceclient, _speed_percent);

    return {std::format(DIC_SKIP_MSG, num_for_skip)};
}

void Bragi::SpeedCommand(const dpp::slashcommand_t &event) { }
