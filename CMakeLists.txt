CMAKE_MINIMUM_REQUIRED(VERSION 3.27)
SET(CMAKE_CXX_STANDARD 23)
PROJECT(Bragi)

INCLUDE_DIRECTORIES(src)
INCLUDE(cmake/dpp.cmake)

ADD_EXECUTABLE(Bragi
    src/master.cpp
    src/coms/error.cpp
    src/coms/play_attachment.cpp
    src/coms/ping.cpp
    src/coms/join.cpp
    src/util/logger.cpp
    src/util/path.cpp
    src/util/settings.cpp
    src/util/color.cpp
    src/coms/leave.cpp
    src/guild/guild.cpp
)

TARGET_LINK_LIBRARIES(Bragi dpp oggz)