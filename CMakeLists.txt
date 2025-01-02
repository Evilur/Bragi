CMAKE_MINIMUM_REQUIRED(VERSION 3.30.2)
SET(CMAKE_CXX_STANDARD 26)
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

    # Guild player and its components
    src/player/track/deezer_track.cpp
    src/player/track/track.cpp
    src/player/guild_player.cpp

    # Some different utils
    src/util/logger.cpp
    src/util/path.cpp
    src/util/settings.cpp
    src/util/color.cpp
    src/util/json.cpp
    src/util/parser.cpp
)

TARGET_LINK_LIBRARIES(Bragi opus opusenc speexdsp crypto ssl FLAC++ dpp)