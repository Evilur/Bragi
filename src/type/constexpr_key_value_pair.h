#pragma once

#include "util/class.h"
#include "util/move.h"

/**
 * A simple key->value pair representation
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T, typename U>
class ContexprKeyValuePair final {
public:
    PREVENT_COPY_AND_MOVE(ContexprKeyValuePair);

    consteval ContexprKeyValuePair(T&& key, U&& value) noexcept;

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
ContexprKeyValuePair<T, U>::ContexprKeyValuePair(T&& key, U&& value)
noexcept : _key(key), _value(value) { }

template <typename T, typename U>
consteval inline T& ContexprKeyValuePair<T, U>::Key()
noexcept { return _key; }

template <typename T, typename U>
consteval inline const T& ContexprKeyValuePair<T, U>::Key() const
noexcept { return _key; }

template <typename T, typename U>
consteval inline U& ContexprKeyValuePair<T, U>::Value()
noexcept { return _value; }

template <typename T, typename U>
consteval inline const U& ContexprKeyValuePair<T, U>::Value() const
noexcept { return _value; }
