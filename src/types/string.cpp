#include "types/string.hpp"

#include <cstring>

String::String(const unsigned long str_size) :
    _str(new char[str_size + 1]), _size(str_size) {
    _str[str_size] = '\0';
}

String::String(const char* const str) : String(str, strlen(str)) { }

String::String(const char* str, const unsigned long str_size) :
    String(str_size) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
    strncpy(_str, str, str_size);
#pragma GCC diagnostic pop
}

String::~String() {
    delete[] _str;
}

String String::operator+(const char symbol) const noexcept {
    String result(_str, _size + 1);
    result._str[_size] = symbol;
    return result;
}

String::operator const char*() const noexcept {
    return _str;
}

String::operator std::string_view() const {
    return { _str };
}

unsigned char String::ToUInt8(const char* str) {
    return ToUInt<unsigned char>(str);
}

unsigned short String::ToUInt16(const char* str) {
    return ToUInt<unsigned short>(str);
}

unsigned int String::ToUInt32(const char* str) {
    return ToUInt<unsigned int>(str);
}

unsigned long String::ToUInt64(const char* str) {
    return ToUInt<unsigned long>(str);
}

char String::ToInt8(const char* str) {
    return ToInt<char, unsigned char>(str);
}

short String::ToInt16(const char* str) {
    return ToInt<short, unsigned short>(str);
}

int String::ToInt32(const char* str) {
    return ToInt<int, unsigned int>(str);
}

long String::ToInt64(const char* str) {
    return ToInt<long, unsigned long>(str);
}
