#ifndef BRAGI_LOGGER_H
#define BRAGI_LOGGER_H

#include "master.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

#define LOG_LEVEL 0

#define TRACE_LOG(msg)
#define DEBUG_LOG(msg)
#define INFO_LOG(msg)
#define WARN_LOG(msg)
#define ERROR_LOG(msg)
#define FATAL_LOG(msg)

#if LOG_LEVEL == 0
#undef TRACE_LOG
#define TRACE_LOG(msg) Logger::Log(msg, Logger::TRACE, stdout);
#endif
#if LOG_LEVEL <= 1
#undef DEBUG_LOG
#define DEBUG_LOG(msg) Logger::Log(msg, Logger::DEBUG, stdout);
#endif
#if LOG_LEVEL <= 2
#undef INFO_LOG
#define INFO_LOG(msg) Logger::Log(msg, Logger::INFO, stdout);
#endif
#if LOG_LEVEL <= 3
#undef WARN_LOG
#define WARN_LOG(msg) Logger::Log(msg, Logger::WARN, stdout);
#endif
#if LOG_LEVEL <= 4
#undef ERROR_LOG
#define ERROR_LOG(msg) Logger::Log(msg, Logger::ERROR, stderr);
#endif
#if LOG_LEVEL <= 5
#undef FATAL_LOG
#define FATAL_LOG(msg) Logger::Log(msg, Logger::FATAL, stderr);
#endif

/* Class for logging important messages */
class Logger final {
public:
    /* Enumerator with log levels */
    enum LogLevel : byte { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };

    /* Master log method */
    template<typename T>
    static void Log(T message, LogLevel log_level, FILE* stream) {
        /* Get current time */
        char time_buffer[20];
        time_t current_time = time(nullptr);
        tm* time_info = localtime(&current_time);
        int offset = strftime(time_buffer, sizeof(time_buffer),
                              "%Y.%m.%d %H:%M:%S", time_info);

		/* Log the message */
        fprintf(stream, "%s[%s] %s %s\n",
               COLOR_CODE_STR[log_level],
               time_buffer,
               LOG_LEVEL_STR[log_level],
               message);
	}

private:
	static constexpr char LOG_LEVEL_STR[6][8] = {
        "Trace: ", "Debug: ", "Info:  ", "Warn:  ", "Error: ", "Fatal: "
    };
	static constexpr char COLOR_CODE_STR[6][6] = {
        "\e[37m", "\e[35m", "\e[34m", "\e[33m", "\e[31m", "\e[31m"
    };
};

#endif
