#pragma once

#include "type/constexpr_key_value_pair.h"

#include <cstdint>

/**
 * A compile-time Radix (Compressed) trie implementation
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T, uint32_t N>
class ConstexprRadixTrie final {
public:
    consteval ConstexprRadixTrie(
        const ContexprKeyValuePair<const char*, T> (&entries)[N]
    ) noexcept;
};

template<typename T, uint32_t N>
ConstexprRadixTrie(
    const ContexprKeyValuePair<const char*, T> (&entries)[N]
) noexcept
    -> ConstexprRadixTrie<T, N>;

template <typename T, uint32_t N>
inline consteval ConstexprRadixTrie<T, N>::ConstexprRadixTrie(
    const ContexprKeyValuePair<const char*, T> (&entries)[N]
) noexcept {
}
