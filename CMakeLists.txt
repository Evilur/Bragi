CMAKE_MINIMUM_REQUIRED(VERSION 3.25)
SET(CMAKE_CXX_STANDARD 23)
PROJECT(Bragi)

INCLUDE_DIRECTORIES(src)

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