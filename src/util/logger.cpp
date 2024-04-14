#include "logger.h"
#include "util/path.h"

#include <iostream>
#include <filesystem>
#include <ctime>
#include <set>

void Logger::Init() {
	fs::create_directory(Path::LOG_DIR);
	const std::string name = Date() + ".log";
	fs::path log_file = Path::LOG_DIR / name;
	_stream = new std::ofstream(log_file);
	std::cout << "Log file: " << log_file.c_str() << std::endl;
	CleanLogs();
}

void Logger::CleanLogs() {
	char counter = 0;
	std::set<fs::path> list;
	for (const fs::path entry : fs::directory_iterator(Path::LOG_DIR)) list.insert(entry);
	for (const fs::path &log_file : list) if (list.size() - counter++ > 15) std::remove(log_file.c_str());
}


/* Example: 2023.10.21 17:05:55 */
std::string Logger::Date() {
	time_t now = time(nullptr);
	tm* tm = localtime(&now);
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