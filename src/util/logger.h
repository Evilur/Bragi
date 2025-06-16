#ifndef BRAGI_LOGGER_H
#define BRAGI_LOGGER_H

#include "master.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

#define TRACE_LOG(msg)
#define DEBUG_LOG(msg)
#define INFO_LOG(msg)
#define WARN_LOG(msg)
#define ERROR_LOG(msg)
#define FATAL_LOG(msg)

#if LOG_LEVEL == 0
#undef TRACE_LOG
#define TRACE_LOG(msg) Logger::Log(msg, Logger::TRACE);
#endif
#if LOG_LEVEL <= 1
#undef DEBUG_LOG
#define DEBUG_LOG(msg) Logger::Log(msg, Logger::DEBUG);
#endif
#if LOG_LEVEL <= 2
#undef INFO_LOG
#define INFO_LOG(msg) Logger::Log(msg, Logger::INFO);
#endif
#if LOG_LEVEL <= 3
#undef WARN_LOG
#define WARN_LOG(msg) Logger::Log(msg, Logger::WARN);
#endif
#if LOG_LEVEL <= 4
#undef ERROR_LOG
#define ERROR_LOG(msg) Logger::Log(msg, Logger::ERROR);
#endif
#if LOG_LEVEL <= 5
#undef FATAL_LOG
#define FATAL_LOG(msg) Logger::Log(msg, Logger::FATAL);
#endif

/* Class for logging important messages */
class Logger final {
public:
	/* Enumerator with log levels */
	enum LogLevel : byte { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };

	/* Init the logger (for filesystem) */
	static void Init();

	template<typename T>
	static void Trace(T message) { Log(message, TRACE); }

	template<typename T>
	static void Debug(T message) { Log(message, DEBUG); }

	template<typename T>
	static void Info(T message) { Log(message, INFO); }

	template<typename T>
	static void Warn(T message) { Log(message, WARN); }

	template<typename T>
	static void Error(T message) { Log(message, ERROR); }

	template<typename T>
	static void Fatal(T message) { Log(message, FATAL); }

	/* Master log method */
	template<typename T>
	static void Log(T message, LogLevel log_level) {
		/* Get the prefix. Format: '\e[35m[1970.01.01 00:00:00] DEBUG: ' */
		char prefix[30];
		sprintf(prefix, "%s[%s] %s", COLOR_CODE_STR[log_level], GetDate(), LOG_LEVEL_STR[log_level]);

		/* Log the message */
		std::cout << prefix << message << std::endl;
	}

private:
	/* Date pointer (20 chars is enough to record the date and time: 2024.09.17 18:51:47 + \0) */
	inline static char _current_date[20] = "1970.01.01 00:00:00";

	/* Stock logging hideouts */
	static constexpr char LOG_LEVEL_STR[6][8] = { "Trace: ", "Debug: ", "Info:  ", "Warn:  ", "Error: ", "Fatal: " };
	static constexpr char COLOR_CODE_STR[6][6] = { "\e[37m", "\e[35m", "\e[34m", "\e[33m", "\e[31m", "\e[31m" };

	/* Get the current formatted date */
	static const char* GetDate();
};

#endif
