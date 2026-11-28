#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>

class String final {
public:
    constexpr String() noexcept;

    constexpr explicit String(uint64_t string_len);

    template <typename T,
        std::enable_if_t<std::is_same_v<T, const char*> ||
        std::is_same_v<T, char*>, int> = 0>
    constexpr String(T string);

    template <uint32_t N>
    constexpr String(const char (&string)[N]);

    constexpr String(const char* string, uint64_t string_len);

    constexpr String(const String& other);

    constexpr String& operator=(const String& other);

    constexpr String(String&& other) noexcept;

    constexpr String& operator=(String&& other) noexcept;

    constexpr ~String() noexcept;

    [[nodiscard]]
    constexpr String operator+(char symbol) const;

    [[nodiscard]]
    constexpr String operator+(const char* string) const;

    [[nodiscard]]
    constexpr bool operator==(const String& other) const noexcept;

    template <typename T,
        std::enable_if_t<std::is_same_v<T, const char*> ||
        std::is_same_v<T, char*>, int> = 0>
    [[nodiscard]]
    constexpr bool operator==(T other) const noexcept;

    template <uint32_t N>
    [[nodiscard]]
    constexpr bool operator==(const char (&other)[N]) const noexcept;

    [[nodiscard]]
    constexpr bool operator!=(const String& other) const noexcept;

    template <typename T,
        std::enable_if_t<std::is_same_v<T, const char*> ||
        std::is_same_v<T, char*>, int> = 0>
    [[nodiscard]]
    constexpr bool operator!=(T other) const noexcept;

    template <uint32_t N>
    [[nodiscard]]
    constexpr bool operator!=(const char (&other)[N]) const noexcept;

    [[nodiscard]]
    constexpr operator const char*() const noexcept;

    [[nodiscard]]
    constexpr operator char*() const noexcept;

    [[nodiscard]]
    constexpr operator std::string_view() const noexcept;

    [[nodiscard]]
    constexpr uint32_t Length() const noexcept;

    [[nodiscard]]
    constexpr bool IsEmpty() const noexcept;

    constexpr void ToLower() noexcept;

    constexpr void ToUpper() noexcept;

    [[nodiscard]]
    constexpr const char* CStr() const noexcept;

private:
    char* _string;
    uint32_t _len;

public:
    template<typename... Args>
    [[nodiscard]]
    static String Format(const char* format, Args... args);

    template <typename T>
    [[nodiscard]]
    static constexpr T ToInt(const char* string) noexcept;

    template <typename T>
    [[nodiscard]]
    static constexpr T ToInt(
        const char* string, const char*& after_ptr
    ) noexcept;

    template <typename T>
    [[nodiscard]]
    static constexpr T ToInt(
        const char* string, uint32_t len
    ) noexcept;

    template <typename T>
    [[nodiscard]]
    static constexpr T ToInt(
        const char* string, uint32_t len, const char*& after_ptr
    ) noexcept;

    template <typename T>
    [[nodiscard]]
    static constexpr T HexToInt(const char* string) noexcept;

    template <typename T>
    [[nodiscard]]
    static constexpr T HexToInt(
        const char* string, const char*& after_ptr
    ) noexcept;

    template <typename T>
    [[nodiscard]]
    static constexpr T HexToInt(
        const char* string, uint32_t len
    ) noexcept;

    template <typename T>
    [[nodiscard]]
    static constexpr T HexToInt(
        const char* string, uint32_t len, const char*& after_ptr
    ) noexcept;

    [[nodiscard]]
    static constexpr int32_t Compare(const char* x1, const char* x2) noexcept;

    [[nodiscard]]
    static constexpr int32_t Compare(
        const char* x1, const char* x2, uint32_t len
    ) noexcept;

    [[nodiscard]]
    static constexpr bool Equal(const char* x1, const char* x2) noexcept;

    [[nodiscard]]
    static constexpr bool Equal(
        const char* x1, const char* x2, uint32_t len
    ) noexcept;

    [[nodiscard]]
    static constexpr bool EqualIgnoreCase(
        const char* x1, const char* x2
    ) noexcept;

    [[nodiscard]]
    static constexpr bool EqualIgnoreCase(
        const char* x1, const char* x2, uint32_t len
    ) noexcept;

    [[nodiscard]]
    static constexpr uint32_t Length(const char* string) noexcept;

private:
    static constexpr void Copy(
        char* const dest, const char* const src, const uint32_t size
    ) noexcept;
};

inline constexpr String::String() noexcept : _string(nullptr), _len(0) { }

inline constexpr String::String(const uint64_t string_len) :
    _string(new char[string_len + 1]), _len(string_len) {
    _string[string_len] = '\0';
}

template <typename T,
    std::enable_if_t<std::is_same_v<T, const char*> ||
    std::is_same_v<T, char*>, int>>
inline constexpr String::String(const T string)
    : String(string, string != nullptr ? Length(string) : 0) { }

template <uint32_t N>
inline constexpr String::String(const char (&string)[N])
: String(string, N - 1) { }

inline constexpr String::String(const char* string, const uint64_t string_len)
    : String(string_len) {
    if (string != nullptr) Copy(_string, string, string_len);
}

inline constexpr String::String(const String& other) :
    _string(new char[other._len + 1]), _len(other._len) {
    /* Assign a new data */
    Copy(_string, other._string, other._len);
    _string[_len] = '\0';
}

inline constexpr String& String::operator=(const String& other) {
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
        Copy(_string, other._string, other._len + 1);
        _len = other._len;
    }

    return *this;
}

inline constexpr String::String(String&& other) noexcept :
    _string(other._string), _len(other._len) {
    other._string = nullptr;
    other._len = 0;
}

inline constexpr String& String::operator=(String&& other) noexcept {
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

inline constexpr String String::operator+(const char symbol) const {
    String result(_string, _len + 1);
    result._string[_len] = symbol;
    return result;
}

inline constexpr String String::operator+(const char* const other) const {
    /* Nullptr check */
    if (other == nullptr) return String(_string, _len);

    const uint64_t other_len = Length(other);
    String result(_len + other_len);
    Copy(result._string, _string, _len);
    Copy(result._string + _len, other, other_len);
    return result;
}

inline constexpr bool String::operator==(const String& other) const noexcept {
    return _len == other._len && Equal(_string, other._string, _len);
}

template <typename T,
    std::enable_if_t<std::is_same_v<T, const char*> ||
    std::is_same_v<T, char*>, int>>
inline constexpr bool String::operator==(const T other) const
noexcept { return Equal(_string, other); }

template <uint32_t N>
inline constexpr bool String::operator==(const char (&other)[N]) const
noexcept { return _len == N - 1 && Equal(_string, other, N - 1); }

inline constexpr bool String::operator!=(const String& other) const noexcept {
    return _len != other._len || Equal(_string, other._string, _len);
}

template <typename T,
    std::enable_if_t<std::is_same_v<T, const char*> ||
    std::is_same_v<T, char*>, int>>
inline constexpr bool String::operator!=(const T other) const
noexcept { return Equal(_string, other); }

template <uint32_t N>
inline constexpr bool String::operator!=(const char (&other)[N]) const
noexcept { return _len != N - 1 || Equal(_string, other, N - 1); }

inline constexpr String::operator const char*() const
noexcept { return _string; }

inline constexpr String::operator char*() const noexcept { return _string; }

inline constexpr String::operator std::string_view()
const noexcept { return { _string, _len }; }

inline constexpr String::~String() noexcept { delete[] _string; }

inline constexpr uint32_t String::Length() const noexcept { return _len; }

inline constexpr bool String::IsEmpty() const noexcept { return _len == 0; }

inline constexpr void String::ToLower() noexcept {
    for (char* string_ptr = _string,
         * const string_end = _string + _len;
         string_ptr < string_end; ++string_ptr)
        if (*string_ptr >= 'A' && *string_ptr <= 'Z')
            *string_ptr += 'a' - 'A';
}

inline constexpr void String::ToUpper() noexcept {
    for (char* string_ptr = _string,
         * const string_end = _string + _len;
         string_ptr < string_end; ++string_ptr)
        if (*string_ptr >= 'a' && *string_ptr <= 'z')
            *string_ptr -= 'a' - 'A';
}

inline constexpr const char* String::CStr() const noexcept { return _string; }

template <typename T>
inline constexpr T String::ToInt(const char* string) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        while (*string >= '0' && *string <= '9') {
            result *= 10;
            result += *string - '0';
            ++string;
        }
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*string == '-') return -ToInt<unsigned_t>(string + 1);
        return ToInt<unsigned_t>(string);
    }
}

template <typename T>
inline constexpr T String::ToInt(
    const char* string, const char*& after_ptr
) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        while (*string >= '0' && *string <= '9') {
            result *= 10;
            result += *string - '0';
            ++string;
        }
        after_ptr = string;
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*string == '-') return -ToInt<unsigned_t>(string + 1, after_ptr);
        return ToInt<unsigned_t>(string, after_ptr);
    }
}

template <typename T>
inline constexpr T String::ToInt(
    const char* string, uint32_t len
) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        while (*string >= '0' && *string <= '9') {
            result *= 10;
            result += *string - '0';
            ++string;
            if (--len == 0) break;
        }
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*string == '-') return -ToInt<unsigned_t>(string + 1, len - 1);
        return ToInt<unsigned_t>(string, len);
    }
}

template <typename T>
inline constexpr T String::ToInt(
    const char* string, uint32_t len, const char*& after_ptr
) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        while (*string >= '0' && *string <= '9') {
            result *= 10;
            result += *string - '0';
            ++string;
            if (--len == 0) break;
        }
        after_ptr = string;
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*string == '-')
            return -ToInt<unsigned_t>(string + 1, len - 1, after_ptr);
        return ToInt<unsigned_t>(string, len, after_ptr);
    }
}

template <typename T>
inline constexpr T String::HexToInt(const char* string) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        for (;;) {
            char c = *string;
            if (c >= '0' && c <= '9') {
                result *= 16;
                result += c - '0';
            } else if ((c |= 0x20) >= 'a' && c <= 'f') {
                result *= 16;
                result += c - 'a' + 10;
            } else break;
            ++string;
        }
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*string == '-') return -HexToInt<unsigned_t>(string + 1);
        return HexToInt<unsigned_t>(string);
    }
}

template <typename T>
inline constexpr T String::HexToInt(
    const char* string, const char*& after_ptr
) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        for (;;) {
            char c = *string;
            if (c >= '0' && c <= '9') {
                result *= 16;
                result += c - '0';
            } else if ((c |= 0x20) >= 'a' && c <= 'f') {
                result *= 16;
                result += c - 'a' + 10;
            } else break;
            ++string;
        }
        after_ptr = string;
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*string == '-') return -HexToInt<unsigned_t>(string + 1, after_ptr);
        return HexToInt<unsigned_t>(string, after_ptr);
    }
}

template <typename T>
inline constexpr T String::HexToInt(
    const char* string, uint32_t len
) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        for (;;) {
            char c = *string;
            if (c >= '0' && c <= '9') {
                result *= 16;
                result += c - '0';
            } else if ((c |= 0x20) >= 'a' && c <= 'f') {
                result *= 16;
                result += c - 'a' + 10;
            } else break;
            ++string;
            if (--len == 0) break;
        }
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*string == '-') return -HexToInt<unsigned_t>(string + 1, len - 1);
        return HexToInt<unsigned_t>(string, len);
    }
}

template <typename T>
inline constexpr T String::HexToInt(
    const char* string, uint32_t len, const char*& after_ptr
) noexcept {
    /* Check for integer */
    static_assert(std::is_integral_v<T>, "<T> must be the integer");

    /* If the integer type is unsigned */
    if constexpr (std::is_unsigned_v<T>) {
        T result = 0;
        for (;;) {
            char c = *string;
            if (c >= '0' && c <= '9') {
                result *= 16;
                result += c - '0';
            } else if ((c |= 0x20) >= 'a' && c <= 'f') {
                result *= 16;
                result += c - 'a' + 10;
            } else break;
            ++string;
            if (--len == 0) break;
        }
        after_ptr = string;
        return result;
    /* If the integer type is signed */
    } else {
        using unsigned_t = std::make_unsigned_t<T>;
        if (*string == '-')
            return -HexToInt<unsigned_t>(string + 1, len - 1, after_ptr);
        return HexToInt<unsigned_t>(string, len, after_ptr);
    }
}

template <typename... Args>
inline String String::Format(
    const char* const format, Args... args
) {
    String result((uint64_t)snprintf(nullptr, 0, format, args...));
    sprintf(result._string, format, args...);
    return result;
}

inline constexpr int32_t String::Compare(
    const char* x1, const char* x2
) noexcept {
    if consteval {
        while (*x1 && *x1 == *x2) ++x1, ++x2;
        return (unsigned char)*x1 - (unsigned char)*x2;
    } else { return strcmp(x1, x2); }
}

inline constexpr int32_t String::Compare(
    const char* x1, const char* x2, uint32_t len
) noexcept {
    if consteval {
        while (len && *x1 && *x1 == *x2) ++x1, ++x2, --len;
        return len ? (unsigned char)*x1 - (unsigned char)*x2 : 0;
    } else { return strncmp(x1, x2, len); }
}

inline constexpr bool String::Equal(
    const char* x1, const char* x2
) noexcept { return Compare(x1, x2) == 0; }

inline constexpr bool String::Equal(
    const char* x1, const char* x2, uint32_t len
) noexcept { return Compare(x1, x2, len) == 0; }

inline constexpr bool String::EqualIgnoreCase(
    const char* x1, const char* x2
) noexcept {
    if (x1 == x2) return true;
    if (x1 == nullptr || x2 == nullptr) return false;

    for(;; ++x1, ++x2) {
        const unsigned char c1 = (unsigned char)*x1;
        const unsigned char c2 = (unsigned char)*x2;

        if (c1 == c2) {
            if (c1 == '\0') return true;
        } else if (!(((c1 ^ c2) == 0x20) &
                 ((unsigned char)((c1 | 0x20) - 'a') < 26)))
            return false;
    }
}

inline constexpr bool String::EqualIgnoreCase(
    const char* x1, const char* x2, uint32_t len
) noexcept {
    if (len == 0 || x1 == x2) return true;
    if (x1 == nullptr || x2 == nullptr) return false;

    for(; len > 0; ++x1, ++x2, --len) {
        const unsigned char c1 = (unsigned char)*x1;
        const unsigned char c2 = (unsigned char)*x2;

        if (c1 == c2) {
            if (c1 == '\0') return true;
        } else if (!(((c1 ^ c2) == 0x20) &
                 ((unsigned char)((c1 | 0x20) - 'a') < 26)))
            return false;
    }
    return true;
}

inline constexpr uint32_t String::Length(const char* const string) noexcept {
    if consteval {
        uint32_t len = 0;
        while (string[len]) ++len;
        return len;
    } else { return strlen(string); }
}

inline constexpr void String::Copy(
    char* const dest, const char* const src, const uint32_t size
) noexcept {
    if consteval {
        for (uint32_t i = 0; i < size; ++i)
            dest[i] = src[i];
    } else { memcpy(dest, src, size); }
}
