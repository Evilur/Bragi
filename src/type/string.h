#pragma once

#include "util/macro.h"

#include <cstdint>
#include <cstring>
#include <string_view>

class String final {
public:
    String() = delete;

    explicit String(uint64_t str_size);

    String(const char* str);

    String(const char* str, uint64_t str_size);

    String(const String& other);

    String& operator=(const String& other);

    String(String&& other) noexcept;

    String& operator=(String&& other) noexcept;

    String operator+(char symbol) const;

    String operator+(const char* str) const;

    bool operator==(const String& other) const noexcept;

    bool operator==(const char* other) const noexcept;

    bool operator!=(const String& other) const noexcept;

    bool operator!=(const char* other) const noexcept;

    operator const char*() const noexcept;

    operator char*() const noexcept;

    operator std::string_view() const noexcept;

    ~String() noexcept;

    [[nodiscard]] const char* CStr() const noexcept;

private:
    char* _str;
    uint64_t _size;

public:
    template<typename... Args>
    static String Format(const char* format, Args... args);

    template <typename T>
    static T ToInt(const char* str) noexcept;
};

FORCE_INLINE
String::String(const uint64_t str_size) :
    _str(new char[str_size + 1]), _size(str_size) {
    _str[str_size] = '\0';
}

FORCE_INLINE
String::String(const char* const str) : String(str, strlen(str)) { }

FORCE_INLINE
String::String(const char* str, const uint64_t str_size) :
    String(str_size) {
    if (str != nullptr) memcpy(_str, str, str_size);
}

FORCE_INLINE
String::String(const String& other) :
    _str(new char[other._size + 1]), _size(other._size) {
    /* Assign a new data */
    memcpy(_str, other._str, other._size + 1);
}

FORCE_INLINE
String& String::operator=(const String& other) {
    /* Self-assignment check */
    if (this == &other) return *this;

    /* Delete previous data */
    delete[] _str;

    /* Nullptr check */
    if (other._str == nullptr) {
        /* Assign an empty string */
        _str = new char[1];
        _str[0] = '\0';
        _size = 0;
    } else {
        /* Assign a new data */
        _str = new char[other._size + 1];
        memcpy(_str, other._str, other._size + 1);
        _size = other._size;
    }

    return *this;
}

FORCE_INLINE
String::String(String&& other) noexcept :
    _str(other._str), _size(other._size) {
    other._str = nullptr;
    other._size = 0;
}

FORCE_INLINE
String& String::operator=(String&& other) noexcept {
    /* Self-move check */
    if (this == &other) return *this;

    /* Delete previous data */
    delete[] _str;

    /* Assign a new data */
    _str = other._str;
    _size = other._size;

    /* Null the old object */
    other._str = nullptr;
    other._size = 0;
    return *this;
}

FORCE_INLINE
String String::operator+(const char symbol) const {
    String result(_str, _size + 1);
    result._str[_size] = symbol;
    return result;
}

FORCE_INLINE
String String::operator+(const char* const other) const {
    /* Nullptr check */
    if (other == nullptr) return String(_str, _size);

    const uint64_t other_size = strlen(other);
    String result(_size + other_size);
    memcpy(result._str, _str, _size);
    memcpy(result._str + _size, other, other_size);
    return result;
}

FORCE_INLINE
bool String::operator==(const String& other) const noexcept {
    return strcmp(_str, other._str) == 0;
}

FORCE_INLINE
bool String::operator==(const char* const other) const noexcept {
    return strcmp(_str, other) == 0;
}

FORCE_INLINE
bool String::operator!=(const String& other) const noexcept {
    return strcmp(_str, other._str) != 0;
}

FORCE_INLINE
bool String::operator!=(const char* const other) const noexcept {
    return strcmp(_str, other) != 0;
}

FORCE_INLINE
String::operator const char*() const noexcept { return _str; }

FORCE_INLINE
String::operator char*() const noexcept { return _str; }

FORCE_INLINE
String::operator std::string_view()
const noexcept { return { _str, _size }; }

FORCE_INLINE
String::~String() noexcept { delete[] _str; }

FORCE_INLINE
const char* String::CStr() const noexcept { return _str; }

template <typename T>
FORCE_INLINE
T String::ToInt(const char* str) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        while (*str >= '0' && *str <= '9') {
            result *= 10;
            result += *str++ - '0';
        }
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*str == '-') return -ToInt<unsigned_t>(str + 1);
        return ToInt<unsigned_t>(str);
    }
}

template <typename... Args>
FORCE_INLINE
String String::Format(const char* const format, Args... args) {
    String result((uint64_t)snprintf(nullptr, 0, format, args...));
    sprintf(result._str, format, args...);
    return result;
}
