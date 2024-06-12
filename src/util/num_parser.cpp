#include <iostream>
#include "num_parser.h"

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

unsigned int NumParser::HexToDec(const char* data) {
	unsigned int result = 0;
	while (true) {
		if (*data >= '0' && *data <= '9') result = result * 16 + (*data++ - '0');
		else if (*data >= 'A' && *data <= 'F') result = result * 16 + (*data++ - ('A' - 10));
		else return result;
	}
}