#ifndef BRAGI_NUM_PARSER_H
#define BRAGI_NUM_PARSER_H

class NumParser final {
public:
	static unsigned short ToUInt16(const char* data);

	static short ToInt16(const char* data);

	static unsigned int ToUInt32(const char* data);

	static int ToInt32(const char* data);

	static unsigned int ToUInt64(const char* data);

	static int ToInt64(const char* data);

	static unsigned int HexToDec(const char* data);
};

#endif