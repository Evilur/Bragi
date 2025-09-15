CMAKE_MINIMUM_REQUIRED(VERSION 3.25)
SET(CMAKE_CXX_STANDARD 23)
PROJECT(Bragi)

INCLUDE_DIRECTORIES(src)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "-g3 -O0")
ELSE ()
    SET(CMAKE_CXX_FLAGS "-g0 -O2")
ENDIF ()

FILE(GLOB_RECURSE SOURCES "src/*.cpp")

ADD_EXECUTABLE(Bragi ${SOURCES})

TARGET_LINK_LIBRARIES(Bragi
        avcodec
        avformat
        avutil
        crypto
        dpp
        opus
        ssl
        swresample
        swscale)