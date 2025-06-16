#include "logger.h"
#include "util/path.h"

#include <iostream>
#include <filesystem>
#include <ctime>
#include <set>

#define LOG_LEVEL 0
#define DPP_LOG_LEVEL 0
#define DPP_LOG_ENABLED false

const char* Logger::GetDate() {
	/* Output example: 2023.10.21 17:05:55 */
	time_t now = time(nullptr);
	tm* time = localtime(&now);

	auto write_format_unit = [](char* current_date_ptr, const int unit) {
		if (unit < 10) {
			*current_date_ptr++ = '0';
			*current_date_ptr = unit + '0';
			return;
		}
		std::to_chars(current_date_ptr, current_date_ptr + 2, unit, 10);
	};

	std::to_chars(_current_date, _current_date + 4, 1900 + time->tm_year, 10);
	write_format_unit(_current_date + 5, time->tm_mon + 1);
	write_format_unit(_current_date + 8, time->tm_mday);
	write_format_unit(_current_date + 11, time->tm_hour);
	write_format_unit(_current_date + 14, time->tm_min);
	write_format_unit(_current_date + 17, time->tm_sec);

	return _current_date;
}
