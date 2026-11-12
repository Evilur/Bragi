#pragma once

#include "util/macro.h"

#include <cstdint>
#include <cstdio>
#include <ctime>

#define LOG_LEVEL 0

#if LOG_LEVEL == 0
#define TRACE_LOG(...) Logger::Log(stdout, Logger::TRACE_LEVEL, __VA_ARGS__)
#else
#define TRACE_LOG(...)
#endif

#if LOG_LEVEL <= 1
#define DEBUG_LOG(...) Logger::Log(stdout, Logger::DEBUG_LEVEL, __VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

#if LOG_LEVEL <= 2
#define INFO_LOG(...) Logger::Log(stdout, Logger::INFO_LEVEL, __VA_ARGS__)
#else
#define INFO_LOG(...)
#endif

#if LOG_LEVEL <= 3
#define WARN_LOG(...) Logger::Log(stdout, Logger::WARN_LEVEL, __VA_ARGS__)
#else
#define WARN_LOG(...)
#endif

#if LOG_LEVEL <= 4
#define ERROR_LOG(...) Logger::Log(stderr, Logger::ERROR_LEVEL, __VA_ARGS__)
#else
#define ERROR_LOG(...)
#endif

#if LOG_LEVEL <= 5
#define FATAL_LOG(...) Logger::Log(stderr, Logger::FATAL_LEVEL, __VA_ARGS__)
#else
#define FATAL_LOG(...)
#endif

class Logger final {
public:
    enum LogLevel : char {
        TRACE_LEVEL,
        DEBUG_LEVEL,
        INFO_LEVEL,
        WARN_LEVEL,
        ERROR_LEVEL,
        FATAL_LEVEL
    };

    template <typename... Args>
    static void Log(FILE* stream,
                    LogLevel log_level,
                    const char* format,
                    Args... args);

private:
    static constexpr const char* const COLOR_CODE_STR[] = {
        "\033[0m", "\033[35m", "\033[34m", "\033[33m", "\033[31m", "\033[31m"
    };

    static constexpr const char* const LOG_LEVEL_STR[] = {
        "Trace: ", "Debug: ", "Info:  ", "Warn:  ", "Error: ", "Fatal: "
    };
};

template <typename... Args>
FORCE_INLINE void Logger::Log(FILE* const stream,
                              const LogLevel log_level,
                              const char* const format,
                              Args... args) {
    /* Get current time */
    constexpr int32_t time_buffer_size = 20;
    char time_buffer[time_buffer_size];
    const time_t current_time = time(nullptr);
    const tm* const time_info = localtime(&current_time);
    strftime(time_buffer, sizeof(time_buffer),
             "%Y.%m.%d %H:%M:%S", time_info);

    /* Print the prefix */
    fprintf(stream, "%s[%s] %s",
            COLOR_CODE_STR[log_level],
            time_buffer,
            LOG_LEVEL_STR[log_level]);

    /* Print the message */
    if constexpr (sizeof...(Args) > 0)
        fprintf(stream, format, args...);
    else
        fputs(format, stream);

    /* Print a new line char */
    fputc('\n', stream);
}
