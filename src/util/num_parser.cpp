#include <iostream>
#include "num_parser.h"
#include "dictionary.h"

unsigned short NumParser::ToUInt16(const char* data) {
	unsigned short result = 0;
	while (*data >= '0' && *data <= '9') result = result * 10 + (*data++ - '0');
	return result;
}

short NumParser::ToInt16(const char* data) {
	if (data[0] != '-') return ToUInt16(data);
	else return -ToUInt16(data + 1);
}

unsigned int NumParser::ToUInt32(const char* data) {
	unsigned int result = 0;
	while (*data >= '0' && *data <= '9') result = result * 10 + (*data++ - '0');
	return result;
}

int NumParser::ToInt32(const char* data) {
	if (data[0] != '-') return ToUInt32(data);
	else return -ToUInt32(data + 1);
}

unsigned int NumParser::ToUInt64(const char* data) {
	unsigned long result = 0;
	while (*data >= '0' && *data <= '9') result = result * 10 + (*data++ - '0');
	return result;
}

int NumParser::ToInt64(const char* data) {
	if (data[0] != '-') return ToUInt32(data);
	else return -ToUInt32(data + 1);
}

unsigned int NumParser::HexToDec(const char* data) {
	unsigned int result = 0;
	while (true) {
		if (*data >= '0' && *data <= '9') result = result * 16 + (*data++ - '0');
		else if (*data >= 'A' && *data <= 'F') result = result * 16 + (*data++ - ('A' - 10));
		else return result;
	}
}

std::string NumParser::Time(const unsigned short &all_seconds) {
	/* Get hours, minutes and seconds */
	const unsigned short all_minutes = all_seconds / 60;
	const unsigned short all_hours = all_minutes / 60;

	if (all_hours > 0) return std::format(DIC_TRACK_MORE_THAN_HOUR, all_hours, all_minutes % 60, all_seconds % 60);
	else if (all_minutes > 0) return std::format(DIC_TRACK_LESS_THAN_HOUR, all_minutes, all_seconds % 60);
	else return std::format(DIC_TRACK_LESS_THAN_MINUTE, all_seconds);
	return std::string();
}