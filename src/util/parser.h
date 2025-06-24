#ifndef BRAGI_PARSER_H
#define BRAGI_PARSER_H

#include "master.h"

class Parser final {
public:
	static unsigned char ToUInt8(const char* data);

	static signed char ToInt8(const char* data);

	static unsigned short ToUInt16(const char* data);

	static short ToInt16(const char* data);

	static unsigned int ToUInt32(const char* data);

	static int ToInt32(const char* data);

	static unsigned int ToUInt64(const char* data);

	static int ToInt64(const char* data);

	static unsigned int HexToDec(const char* data);

	static std::string Utf8(const char* data);

	static std::string Time(const unsigned short &all_seconds);
};

#endif