#pragma once

#include "util/class.h"
#include "util/move.h"

/**
 * A simple key->value pair representation
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T, typename U>
class ConstexprKeyValuePair final {
public:
    PREVENT_COPY_AND_MOVE(ConstexprKeyValuePair);

    consteval ConstexprKeyValuePair(T&& key, U&& value) noexcept;

    [[nodiscard]]
    consteval T& Key() noexcept;

    [[nodiscard]]
    consteval const T& Key() const noexcept;

    [[nodiscard]]
    consteval U& Value() noexcept;

    [[nodiscard]]
    consteval const U& Value() const noexcept;

  private:
    T _key;
    U _value;
};

template <typename T, typename U>
consteval inline
ConstexprKeyValuePair<T, U>::ConstexprKeyValuePair(T&& key, U&& value)
noexcept : _key(key), _value(value) { }

template <typename T, typename U>
consteval inline T& ConstexprKeyValuePair<T, U>::Key()
noexcept { return _key; }

template <typename T, typename U>
consteval inline const T& ConstexprKeyValuePair<T, U>::Key() const
noexcept { return _key; }

template <typename T, typename U>
consteval inline U& ConstexprKeyValuePair<T, U>::Value()
noexcept { return _value; }

template <typename T, typename U>
consteval inline const U& ConstexprKeyValuePair<T, U>::Value() const
noexcept { return _value; }
