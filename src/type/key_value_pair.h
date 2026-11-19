#pragma once

#include "util/class.h"
#include "util/move.h"
#include "util/access_traits.h"

/**
 * A simple key->value pair representation
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename TKey, typename TValue>
class KeyValuePair final {
public:
    ALLOW_COPY_ALLOW_MOVE(KeyValuePair);

    constexpr KeyValuePair(const TKey& key, const TValue& value) noexcept;

    constexpr KeyValuePair(TKey&& key, TValue&& value) noexcept;

    [[nodiscard]]
    constexpr const access_t<TKey> Key() const noexcept;

    [[nodiscard]]
    TValue& Value() noexcept;

    [[nodiscard]]
    constexpr const access_t<TValue> Value() const noexcept;

private:
    TKey _key;
    TValue _value;
};

template <typename TKey, typename TValue>
constexpr inline KeyValuePair<TKey, TValue>::KeyValuePair(
    const TKey& key, const TValue& value
) noexcept : _key(key), _value(value) { }

template <typename TKey, typename TValue>
constexpr inline
KeyValuePair<TKey, TValue>::KeyValuePair(TKey&& key, TValue&& value)
noexcept : _key(move(key)), _value(move(value)) { }

template <typename TKey, typename TValue>
constexpr inline
const access_t<TKey> KeyValuePair<TKey, TValue>::Key() const
noexcept { return _key; }

template <typename TKey, typename TValue>
inline TValue& KeyValuePair<TKey, TValue>::Value()
noexcept { return _value; }

template <typename TKey, typename TValue>
constexpr inline
const access_t<TValue> KeyValuePair<TKey, TValue>::Value() const
noexcept { return _value; }
