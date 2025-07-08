#pragma once
#include "util/logger.hpp"

#include <iosfwd>
#include <sstream>

class String final {
public:
    explicit String(unsigned long str_size);

    String(const char* str);

    String(const char* str, unsigned long str_size);

    String(const String&) = delete;
    String(String&&) = default;

    ~String();

    operator const char*() const;

    operator std::string_view() const;

private:
    char* const _str;

public:
    template<typename... Args>
    static String Format(const char* format, Args... args);
};

template <typename... Args>
String String::Format(const char* const format, Args... args) {
    const int string_size = snprintf(nullptr, 0, format, args...);
    String result(string_size);
    sprintf(result._str, format, args...);
    return result;
}