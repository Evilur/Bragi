#ifndef BRAGI_LOGGER_H
#define BRAGI_LOGGER_H

#include <string>
#include <iostream>
#include <fstream>

enum LogLevel : char {	INFO, WARN, DEBUG, FATAL };

/* Class for logging important messages */
class Logger final {
public:
	/* Init the logger (for filesystem) */
	static void Init();
	
	/* Log messages */
	template<typename T>
	static void Info(T message) { Logger::Log(message, INFO); }

	template<typename T>
	static void Warn(T message) { Logger::Log(message, WARN); }

	template<typename T>
	static void Debug(T message) { Logger::Log(message, DEBUG); }

	template<typename T>
	static void Fatal(T message) { Logger::Log(message, FATAL); }
private:
	inline static std::ofstream* _stream = nullptr;  //Log file stream
	
	/* Master log method */
	template<typename T>
	static void Log(T message, LogLevel log_level) {
		std::string color_code_str, log_level_str;
		
		switch (log_level) {
			case INFO:
				color_code_str = "\033[34m";
				log_level_str = " Info]:  ";
				break;

			case WARN:
				color_code_str = "\033[33m";
				log_level_str = " Warn]:  ";
				break;

			case DEBUG:
				color_code_str = "\033[35m";
				log_level_str = " Debug]: ";
				break;

			case FATAL:
				color_code_str = "\033[31m";
				log_level_str = " Fatal]: ";
				break;
		}
		
		
		std::string data = '[' + Logger::Date() + log_level_str;
		std::cout << color_code_str << data << message << "\033[0m" << std::endl;
		*_stream << data << message << std::endl;
	}
	
	/* Clean the old log files */
	static void CleanLogs();
	
	/* Get the current formatted date */
	static std::string Date();
	
	/* Format units, which can starts with zero */
	static std::string UnitFormat(int unit);
};

#endif