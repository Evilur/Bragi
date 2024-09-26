#ifndef BRAGI_LOGGER_H
#define BRAGI_LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

/* Class for logging important messages */
class Logger final {
public:
	/* Init the logger (for filesystem) */
	static void Init();

	template<typename T>
	static void Info(T message) { Log(message, INFO); }

	template<typename T>
	static void Warn(T message) { Log(message, WARN); }

	template<typename T>
	static void Fatal(T message) { Log(message, FATAL); }

	template<typename T>
	static void Debug(T message) { Log(message, DEBUG); }

private:
	/* Enumerator with log levels */
	enum LogLevel : unsigned char { INFO, WARN, FATAL, DEBUG };

	/* Log file stream */
	inline static std::ofstream* _stream = nullptr;

	/* Date pointer (20 chars is enough to record the date and time: 2024.09.17 18:51:47 + \0) */
	inline static char _current_date[20] = "1970.01.01 00:00:00";

	/* Stock logging hideouts */
	static constexpr char LOG_LEVEL_STR[4][10] = { " Info]:  ", " Warn]:  ", " Fatal]: ", " Debug]: " };
	static constexpr char COLOR_CODE_STR[4][6] = { "\033[34m", "\033[33m", "\033[31m", "\033[35m" };

	/* Master log method */
	template<typename T>
	static void Log(T message, LogLevel log_level) {
		std::cout << COLOR_CODE_STR[log_level] << '[' << GetDate() << LOG_LEVEL_STR[log_level] << message << std::endl;
		*_stream << COLOR_CODE_STR[log_level] << '[' << GetDate() << LOG_LEVEL_STR[log_level] << message << std::endl;
	}

	/* Remove the old log files */
	static void CleanLogs();

	/* Get the current formatted date */
	static const char* GetDate();
};

#endif