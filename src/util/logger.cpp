#include "logger.h"
#include <iostream>
#include <filesystem>
#include <ctime>

/* Example: 2023.10.21 17:05:55 */
std::string Logger::Date() {
	time_t now = time(nullptr);
	tm *tm = localtime(&now);
	return std::to_string(1900 + tm->tm_year) + '.' +
	       Logger::UnitFormat(tm->tm_mon + 1) + '.' +
	       Logger::UnitFormat(tm->tm_mday) + ' ' +
	       Logger::UnitFormat(tm->tm_hour) + ':' +
	       Logger::UnitFormat(tm->tm_min) + ':' +
	       Logger::UnitFormat(tm->tm_sec);
}

std::string Logger::UnitFormat(int unit) {
	if (unit < 10) return '0' + std::to_string(unit);
	else return std::to_string(unit);
}
