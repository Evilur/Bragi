#include "types/string.hpp"

#include <cstring>

String::String(const unsigned long str_size) : _str(new char[str_size + 1]) { }

String::String(const char* const str) : String(str, strlen(str)) { }

String::String(const char* str, const unsigned long str_size) :
    _str(new char[str_size + 1]) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
    strncpy(_str, str, str_size);
    _str[str_size] = '\0';
#pragma GCC diagnostic pop
}

String::~String() {
    delete[] _str;
}

String::operator const char*() const {
    return _str;
}

String::operator std::string_view() const {
    return std::string_view(_str);
}
