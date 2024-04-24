CMAKE_MINIMUM_REQUIRED(VERSION 3.27)
SET(CMAKE_CXX_STANDARD 23)
PROJECT(Bragi)

INCLUDE_DIRECTORIES(src)
INCLUDE(cmake/dpp.cmake)

ADD_EXECUTABLE(Bragi
    src/master.cpp
    src/exception/bragi_exception.cpp
    src/exception/http_client_exception.cpp
    src/command/play_attachment.cpp
    src/command/ping.cpp
    src/command/join.cpp
    src/util/logger.cpp
    src/util/path.cpp
    src/util/settings.cpp
    src/util/color.cpp
    src/command/leave.cpp
    src/player/guild_player.cpp
    src/player/attachment_track.cpp
    src/player/track.cpp
    src/web/http_client.cpp
    src/converter/audio_to_opus.cpp
    src/converter/wav_to_opus.cpp
    src/web/https_client.cpp
    src/web/web_client.cpp
    src/player/playlist.cpp
)

TARGET_LINK_LIBRARIES(Bragi dpp opusenc)