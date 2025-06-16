#ifndef BRAGI_LOGGER_H
#define BRAGI_LOGGER_H

#include "master.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdarg>

#define LOG_LEVEL 0

#if LOG_LEVEL == 0
#define TRACE_LOG(...) Logger::Log(stdout, Logger::TRACE, __VA_ARGS__);
#else
#define TRACE_LOG(...)
#endif

#if LOG_LEVEL <= 1
#define DEBUG_LOG(...) Logger::Log(stdout, Logger::DEBUG, __VA_ARGS__);
#else
#define DEBUG_LOG(...)
#endif

#if LOG_LEVEL <= 2
#define INFO_LOG(...) Logger::Log(stdout, Logger::INFO, __VA_ARGS__);
#else
#define INFO_LOG(...)
#endif

#if LOG_LEVEL <= 3
#define WARN_LOG(...) Logger::Log(stdout, Logger::WARN, __VA_ARGS__);
#else
#define WARN_LOG(...)
#endif

#if LOG_LEVEL <= 4
#define ERROR_LOG(...) Logger::Log(stderr, Logger::ERROR, __VA_ARGS__);
#else
#define ERROR_LOG(...)
#endif

#if LOG_LEVEL <= 5
#define FATAL_LOG(...) Logger::Log(stderr, Logger::FATAL, __VA_ARGS__);
#else
#define FATAL_LOG(...)
#endif

class Logger final {
public:
    enum LogLevel {
        TRACE, DEBUG, INFO, WARN, ERROR, FATAL
    };

    static void Log(FILE* stream,
                    const LogLevel log_level,
                    const char* format, ...);

private:
	static constexpr char COLOR_CODE_STR[6][6] = {
        "\e[37m", "\e[35m", "\e[34m", "\e[33m", "\e[31m", "\e[31m"
    };

	static constexpr char LOG_LEVEL_STR[6][8] = {
        "Trace: ", "Debug: ", "Info:  ", "Warn:  ", "Error: ", "Fatal: "
    };
};

#endif
