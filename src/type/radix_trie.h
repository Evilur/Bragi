#pragma once

#include "type/key_value_pair.h"

#include <cstdint>
#include <type_traits>

/**
 * A compile-time Radix (Compressed) trie implementation
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T, uint32_t N>
class RadixTrie final {
public:
    consteval RadixTrie(
        const KeyValuePair<const char*, T> (&entries)[N]
    ) noexcept;

    constexpr access_t<T> Get(const char* key) const noexcept;

private:
    struct Node {
        uint32_t first_edge;
        uint32_t edge_count;
        uint32_t value_index;
    };

    struct Edge {
        const char* key;
        uint32_t length;
        uint32_t child_index;
    };

    const char* keys[N];
    T values[N];

public:
    static consteval int Compare(const char* x1, const char* x2) noexcept {
        while (*x1 != '\0' && *x1 == *x2) { ++x1; ++x2; }
        return (uint8_t)*x1 - (uint8_t)*x2;
    }
};

template <typename T, uint32_t N>
inline consteval RadixTrie<T, N>::RadixTrie(
    const KeyValuePair<const char*, T> (&entries)[N]
) noexcept {
    /* Insert every pair straight into its sorted position.
     * Invariant: keys[0..i) and values[0..i) are already sorted,
     * and key and value of one pair always share the same index */
    for (uint32_t i = 0; i < N; ++i) {
        const char* const key = entries[i].Key();
        const T value = entries[i].Value();

        /* Shift the greater keys one slot to the right */
        uint32_t j = i;
        while (j > 0 && Compare(keys[j - 1], key) > 0) {
            keys[j] = keys[j - 1];
            values[j] = values[j - 1];
            --j;
        }

        /* Put the new pair into the freed slot */
        keys[j] = key;
        values[j] = value;
    }
}

template <typename T, uint32_t N>
inline constexpr
access_t<T> RadixTrie<T, N>::Get(
    const char* const key
) const noexcept {
    for (uint32_t i = 0; i < N; ++i) {
        printf("%s -> %u\n", keys[i], values[i]);
    }
    return 666;
}
