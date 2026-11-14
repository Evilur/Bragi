#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>

class String final {
public:
    String() = delete;

    explicit String(uint64_t string_len);

    template <typename T,
        std::enable_if_t<std::is_same_v<T, const char*> ||
        std::is_same_v<T, char*>, int> = 0>
    String(T string);

    template <uint32_t N>
    String(const char (&string)[N]);

    String(const char* string, uint64_t string_len);

    String(const String& other);

    String& operator=(const String& other);

    String(String&& other) noexcept;

    String& operator=(String&& other) noexcept;

    String operator+(char symbol) const;

    String operator+(const char* string) const;

    bool operator==(const String& other) const noexcept;

    template <typename T,
        std::enable_if_t<std::is_same_v<T, const char*> ||
        std::is_same_v<T, char*>, int> = 0>
    bool operator==(T other) const noexcept;

    template <uint32_t N>
    bool operator==(const char (&other)[N]) const noexcept;

    bool operator!=(const String& other) const noexcept;

    template <typename T,
        std::enable_if_t<std::is_same_v<T, const char*> ||
        std::is_same_v<T, char*>, int> = 0>
    bool operator!=(T other) const noexcept;

    template <uint32_t N>
    bool operator!=(const char (&other)[N]) const noexcept;

    operator const char*() const noexcept;

    operator char*() const noexcept;

    operator std::string_view() const noexcept;

    ~String() noexcept;

    uint32_t Length() const noexcept;

    bool IsEmpty() const noexcept;

    void ToLower() noexcept;

    void ToUpper() noexcept;

    [[nodiscard]] const char* CStr() const noexcept;

private:
    char* _string;
    uint32_t _len;

public:
    template<typename... Args>
    static String Format(const char* format, Args... args);

    template <typename T>
    static T ToInt(const char* string) noexcept;
};

inline String::String(const uint64_t string_len) :
    _string(new char[string_len + 1]), _len(string_len) {
    _string[string_len] = '\0';
}

template <typename T,
    std::enable_if_t<std::is_same_v<T, const char*> ||
    std::is_same_v<T, char*>, int>>
inline String::String(const T string)
    : String(string, string != nullptr ? strlen(string) : 0) { }

template <uint32_t N>
inline String::String(const char (&string)[N]) : String(string, N - 1) { }

inline String::String(const char* string, const uint64_t string_len) :
    String(string_len) {
    if (string != nullptr) memcpy(_string, string, string_len);
}

inline String::String(const String& other) :
    _string(new char[other._len + 1]), _len(other._len) {
    /* Assign a new data */
    memcpy(_string, other._string, other._len + 1);
}

inline String& String::operator=(const String& other) {
    /* Self-assignment check */
    if (this == &other) return *this;

    /* Delete previous data */
    delete[] _string;

    /* Nullptr check */
    if (other._string == nullptr) {
        /* Assign an empty string */
        _string = new char[1];
        _string[0] = '\0';
        _len = 0;
    } else {
        /* Assign a new data */
        _string = new char[other._len + 1];
        memcpy(_string, other._string, other._len + 1);
        _len = other._len;
    }

    return *this;
}

inline String::String(String&& other) noexcept :
    _string(other._string), _len(other._len) {
    other._string = nullptr;
    other._len = 0;
}

inline String& String::operator=(String&& other) noexcept {
    /* Self-move check */
    if (this == &other) return *this;

    /* Delete previous data */
    delete[] _string;

    /* Assign a new data */
    _string = other._string;
    _len = other._len;

    /* Null the old object */
    other._string = nullptr;
    other._len = 0;
    return *this;
}

inline String String::operator+(const char symbol) const {
    String result(_string, _len + 1);
    result._string[_len] = symbol;
    return result;
}

inline String String::operator+(const char* const other) const {
    /* Nullptr check */
    if (other == nullptr) return String(_string, _len);

    const uint64_t other_len = strlen(other);
    String result(_len + other_len);
    memcpy(result._string, _string, _len);
    memcpy(result._string + _len, other, other_len);
    return result;
}

inline bool String::operator==(const String& other) const noexcept {
    return _len == other._len && memcmp(_string, other._string, _len) == 0;
}

template <typename T,
    std::enable_if_t<std::is_same_v<T, const char*> ||
    std::is_same_v<T, char*>, int>>
inline bool String::operator==(const T other) const
noexcept { return strcmp(_string, other) == 0; }

template <uint32_t N>
inline bool String::operator==(const char (&other)[N]) const
noexcept { return _len == N - 1 && memcmp(_string, other, N - 1) == 0; }

inline bool String::operator!=(const String& other) const noexcept {
    return _len != other._len || memcmp(_string, other._string, _len) != 0;
}

template <typename T,
    std::enable_if_t<std::is_same_v<T, const char*> ||
    std::is_same_v<T, char*>, int>>
inline bool String::operator!=(const T other) const
noexcept { return strcmp(_string, other) != 0; }

template <uint32_t N>
inline bool String::operator!=(const char (&other)[N]) const
noexcept { return _len != N - 1 || memcmp(_string, other, N - 1) != 0; }

inline String::operator const char*() const noexcept { return _string; }

inline String::operator char*() const noexcept { return _string; }

inline
String::operator std::string_view()
const noexcept { return { _string, _len }; }

inline String::~String() noexcept { delete[] _string; }

inline uint32_t String::Length() const noexcept { return _len; }

inline bool String::IsEmpty() const noexcept { return _len == 0; }

inline void String::ToLower() noexcept {
    for (char* string_ptr = _string,
         * const string_end = _string + _len;
         string_ptr < string_end; ++string_ptr)
        if (*string_ptr >= 'A' && *string_ptr <= 'Z')
            *string_ptr += 'a' - 'A';
}

inline void String::ToUpper() noexcept {
    for (char* string_ptr = _string,
         * const string_end = _string + _len;
         string_ptr < string_end; ++string_ptr)
        if (*string_ptr >= 'a' && *string_ptr <= 'z')
            *string_ptr -= 'a' - 'A';
}

inline const char* String::CStr() const noexcept { return _string; }

template <typename T>
inline T String::ToInt(const char* string) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        while (*string >= '0' && *string <= '9') {
            result *= 10;
            result += *string++ - '0';
        }
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*string == '-') return -ToInt<unsigned_t>(string + 1);
        return ToInt<unsigned_t>(string);
    }
}

template <typename... Args>
inline String String::Format(const char* const format, Args... args) {
    String result((uint64_t)snprintf(nullptr, 0, format, args...));
    sprintf(result._string, format, args...);
    return result;
}
