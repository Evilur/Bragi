CMAKE_MINIMUM_REQUIRED(VERSION 3.25)
SET(CMAKE_CXX_STANDARD 23)
PROJECT(Bragi)

INCLUDE_DIRECTORIES(src)

ADD_EXECUTABLE(Bragi
    # A master file
    src/master.cpp

    # Interfaces for working with different APIs
    src/client/deezer_client.cpp

    # Slash/mesage commands
    src/command/ping.cpp
    src/command/join.cpp
    src/command/leave.cpp
    src/command/play.cpp
    src/command/list.cpp
    src/command/loop.cpp
    src/command/skip.cpp
    src/command/next.cpp
    src/command/speed.cpp

    # Discord audio senders
    src/sender/opus_sender.cpp

    # Exceptions
    src/exception/bragi_exception.cpp
    src/exception/json_exception.cpp

    # Http clients
    src/http/http_client.cpp
    src/http/https_client.cpp

    # Guild player
    src/guild/guild_player.cpp

    # Tracks
    src/track/track.cpp
    src/track/deezer_track.cpp

    # Some different utils
    src/util/logger.cpp
    src/util/path.cpp
    src/util/settings.cpp
    src/util/color.cpp
    src/util/json.cpp
    src/util/parser.cpp
)

TARGET_LINK_LIBRARIES(Bragi opus opusenc speexdsp crypto ssl FLAC++ dpp)