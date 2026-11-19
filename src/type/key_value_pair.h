#pragma once

#include "util/class.h"
#include "util/move.h"

/**
 * A simple key->value pair representation
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T, typename U>
class KeyValuePair final {
public:
    ALLOW_COPY_ALLOW_MOVE(KeyValuePair);

    constexpr KeyValuePair(const T& key, const T& value);

    constexpr KeyValuePair(T&& key, U&& value);

    [[nodiscard]]
    constexpr const T& Key() const noexcept;

    [[nodiscard]]
    U& Value() noexcept;

    [[nodiscard]]
    constexpr const U& Value() const noexcept;

private:
    T _key;
    U _value;
};

template <typename T, typename U>
constexpr inline KeyValuePair<T, U>::KeyValuePair(const T& key, const T& value)
: _key(key), _value(value) { }

template <typename T, typename U>
constexpr inline
KeyValuePair<T, U>::KeyValuePair(T&& key, U&& value)
: _key(move(key)), _value(move(value)) { }

template <typename T, typename U>
constexpr inline const T& KeyValuePair<T, U>::Key() const
noexcept { return _key; }

template <typename T, typename U>
inline U& KeyValuePair<T, U>::Value()
noexcept { return _value; }

template <typename T, typename U>
constexpr inline const U& KeyValuePair<T, U>::Value() const
noexcept { return _value; }
