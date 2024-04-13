CMAKE_MINIMUM_REQUIRED(VERSION 3.27)
SET(CMAKE_CXX_STANDARD 23)
PROJECT(Bragi)

INCLUDE_DIRECTORIES(src)
INCLUDE(cmake/dpp.cmake)

ADD_EXECUTABLE(Bragi
    src/master.cpp
    src/util/bragi_exception.cpp
    src/command/play_attachment.cpp
    src/command/ping.cpp
    src/command/join.cpp
    src/util/logger.cpp
    src/util/path.cpp
    src/util/settings.cpp
    src/util/color.cpp
    src/command/leave.cpp
    src/player/guild_player.cpp
    src/converter/opus_converter.cpp
    src/converter/wav_to_opus.cpp
    src/player/attachment_track.cpp
    src/player/track.cpp
    src/util/http_client.cpp
)

TARGET_LINK_LIBRARIES(Bragi dpp opusenc boost_system)