#pragma once

#include <string_view>
#include <cstdio>

class String final {
public:
    String() noexcept = delete;

    explicit String(unsigned long str_size) noexcept;

    String(const char* str) noexcept;

    String(const char* str, unsigned long str_size) noexcept;

    String(const String&) noexcept = delete;

    String(String&&) noexcept = default;

    ~String() noexcept;

    String operator+(char symbol) const noexcept;

    operator const char*() const noexcept;

    operator std::string_view() const noexcept;

private:
    char* const _str;
    unsigned long _size;

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