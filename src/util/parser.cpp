#include <iostream>
#include "parser.h"
#include "locale/locale.h"

unsigned char Parser::ToUInt8(const char* data) {
        unsigned char result = 0;
        while (*data >= '0' && *data <= '9') result = result * 10 + (*data++ - '0');
        return result;
}

signed char Parser::ToInt8(const char* data) {
        if (data[0] != '-') return ToUInt8(data);
        else return -ToUInt8(data + 1);
}

unsigned short Parser::ToUInt16(const char* data) {
        unsigned short result = 0;
        while (*data >= '0' && *data <= '9') result = result * 10 + (*data++ - '0');
        return result;
}

short Parser::ToInt16(const char* data) {
        if (data[0] != '-') return ToUInt16(data);
        else return -ToUInt16(data + 1);
}

unsigned int Parser::ToUInt32(const char* data) {
        unsigned int result = 0;
        while (*data >= '0' && *data <= '9') result = result * 10 + (*data++ - '0');
        return result;
}

int Parser::ToInt32(const char* data) {
        if (data[0] != '-') return ToUInt32(data);
        else return -ToUInt32(data + 1);
}

unsigned int Parser::ToUInt64(const char* data) {
        unsigned long result = 0;
        while (*data >= '0' && *data <= '9') result = result * 10 + (*data++ - '0');
        return result;
}

int Parser::ToInt64(const char* data) {
        if (data[0] != '-') return ToUInt32(data);
        else return -ToUInt32(data + 1);
}

unsigned int Parser::HexToDec(const char* data) {
        unsigned int result = 0;
        while (true) {
                if (*data >= '0' && *data <= '9') result = result * 16 + (*data++ - '0');
                else if (*data >= 'A' && *data <= 'F') result = result * 16 + (*data++ - ('A' - 10));
                else if (*data >= 'a' && *data <= 'f') result = result * 16 + (*data++ - ('a' - 10));
                else return result;
        }
}

std::string Parser::Utf8(const char* data) {
        if (*data++ != '\\')
                throw std::logic_error("First symbol must be a backslash");

        if (*data++ != 'u')
                throw std::logic_error("Second symbol must be a 'u' symbol");

        unsigned int num = Parser::HexToDec(data);

        int count;
        if (num <= 0x007F)
                count = 1;
        else if (num <= 0x07FF)
                count = 2;
        else if (num <= 0xFFFF)
                count = 3;
        else if (num <= 0x10FFFF)
                count = 4;
        else
                throw std::logic_error("There is no such utf symbol");

        char result[count];

        if (count > 1) {
                for (int i = count - 1; i > 0; --i) {
                        result[i] = (char)(0x80 | (num & 0x3F));
                        num >>= 6;
                }

                for (int i = 0; i < count; ++i) num |= (1 << (7 - i));
        }

        result[0] = (char)num;

        return std::string(result, count);
}

