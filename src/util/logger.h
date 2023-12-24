#ifndef ENIGMA_LOGGER_H
#define ENIGMA_LOGGER_H

#include <string>
#include <iostream>

enum LogLevel : char {	Info, Warn, Debug, Fatal };

/* Class for logging important messages */
class Logger {
public:
	template<typename T>
	static void Info(T message) { Logger::Log(message, LogLevel::Info); }

	template<typename T>
	static void Warn(T message) { Logger::Log(message, LogLevel::Warn); }

	template<typename T>
	static void Debug(T message) { Logger::Log(message, LogLevel::Debug); }

	template<typename T>
	static void Fatal(T message) { Logger::Log(message, LogLevel::Fatal); }
private:
	template<typename T>
	static void Log(T message, LogLevel logLevel) {
		std::string color_code, log_level;
		
		switch (logLevel) {
			case LogLevel::Info:
				color_code = "\033[34m";
				log_level = " Info]:  ";
				break;

			case LogLevel::Warn:
				color_code = "\033[33m";
				log_level = " Warn]:  ";
				break;

			case LogLevel::Debug:
				color_code = "\033[35m";
				log_level = " Debug]: ";
				break;

			case LogLevel::Fatal:
				color_code = "\033[31m";
				log_level = " Fatal]: ";
				break;
		}
		
		
		std::string data = '[' + Logger::Date() + log_level;
		std::cout << color_code << data << message << "\033[0m" << std::endl;
	}
	
	static std::string Date();
	static std::string UnitFormat(int unit);
};

#endif