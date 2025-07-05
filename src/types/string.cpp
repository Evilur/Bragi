#include "types/string.h"

#include <cstring>

String::String(const char* const str) : String(str, strlen(str)) { }

String::String(const unsigned long str_size) : _str(new char[str_size + 1]) { }

String::String(const char* str, const unsigned long str_size) :
    String(str_size) {
    strncpy(_str, str, str_size);
}

String::~String() {
    delete[] _str;
}

String::operator const char*() const {
    return _str;
}
