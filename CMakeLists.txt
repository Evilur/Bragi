CMAKE_MINIMUM_REQUIRED(VERSION 3.25)
SET(CMAKE_CXX_STANDARD 23)
PROJECT(Bragi)

# Set compile options
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "-g3 -O0 \
        -fdiagnostics-color=always \
        -fsanitize=address \
        -fno-omit-frame-pointer")
    SET(CMAKE_CXX_SCAN_FOR_MODULES OFF)
    SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
ELSE ()
    SET(CMAKE_CXX_FLAGS "-g0 -O3 -static \
        -march=native -mtune=native \
        -fomit-frame-pointer -Winline \
        -fdiagnostics-color=always \
        -finline-functions \
        -finline-limit=99999999999999 \
        -finline-small-functions \
        --inline-limit=99999999999999 \
        --param large-stack-frame=2147483647 \
        --param large-stack-frame-growth=2147483647 \
        --param max-inline-insns-single=2147483647 \
        --param max-inline-insns-auto=2147483647 \
        -flto=128")
ENDIF ()

# Search header files in the 'src' directory
INCLUDE_DIRECTORIES(src)

# Add the executable
ADD_EXECUTABLE(Bragi src/main.cpp)

# Link libraries
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
