CMAKE_MINIMUM_REQUIRED(VERSION 3.27)
SET(CMAKE_CXX_STANDARD 23)
PROJECT(Bragi)

INCLUDE_DIRECTORIES(src)

ADD_EXECUTABLE(Bragi
    # A master file
    src/master.cpp

    # Interfaces for working with different APIs
    src/client/deezer_client.cpp

    # Slash/mesage commands
    src/command/play_attachment.cpp
    src/command/ping.cpp
    src/command/join.cpp
    src/command/leave.cpp
    src/command/play.cpp

    # Audio to OPUS convertters
    src/converter/audio_to_opus.cpp
    src/converter/wav_to_opus.cpp

    # Exceptions
    src/exception/bragi_exception.cpp
    src/exception/json_exception.cpp

    # Http clients
    src/http/http_client.cpp
    src/http/https_client.cpp

    # Guild player and its components
    src/player/track/attachment_track.cpp
    src/player/track/deezer_track.cpp
    src/player/track/track.cpp
    src/player/guild_player.cpp
    src/player/playlist.cpp

    # Some different utils
    src/util/logger.cpp
    src/util/path.cpp
    src/util/settings.cpp
    src/util/color.cpp
    src/util/json.cpp
    src/util/num_parser.cpp
)

TARGET_LINK_LIBRARIES(Bragi opus sodium opusenc crypto ssl dpp)