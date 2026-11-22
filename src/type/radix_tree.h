#pragma once

#include "type/queue.h"
#include "util/class.h"
#include "type/key_value_pair.h"
#include "type/string.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <meta>
#include <span>

/**
 * A compile-time Radix (Compressed) Tree implementation
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T = void>
class RadixTree final {
private:
    using NextCharFunc = char (*)(const char*&);

    static constexpr char NextChar(const char*& key) noexcept;

public:
    PREVENT_COPY_ALLOW_MOVE(RadixTree);

    template <uint32_t N>
    constexpr RadixTree(
        const KeyValuePair<const char*, T> (&entries)[N]
    ) noexcept;

    constexpr ~RadixTree() noexcept = default;

    template <NextCharFunc NextChar = NextChar>
    constexpr const T* Get(const char* key) const noexcept;

    template <NextCharFunc NextChar = NextChar>
    constexpr const T* Get(const char* key, uint32_t len) const noexcept;

private:
    struct Node {
        const char* label = nullptr;
        uint32_t first_child = 0;
        uint16_t childs_num = 0;
        bool is_terminal = false;
        uint32_t value_index = 0;
    };

    const T* _values;
    const Node* _nodes;
};

template <typename T>
inline constexpr char RadixTree<T>::NextChar(const char*& key)
noexcept { return *key++; }

template <typename T>
template <uint32_t N>
inline constexpr RadixTree<T>::RadixTree(
    const KeyValuePair<const char*, T> (&entries)[N]
) noexcept {
    static_assert(N > 0, "RadixTrie must contain at least one entry");

    /* ---------- Step 1. Sorting ---------- */

    /* Sorted keys and values.
     * Local: labels point into the static string literals,
     * so the array is not needed after the construction */
    const char* keys[N];
    T values[N];

    /* Insertion sort of the pairs by key. Keys with a common prefix
     * become contiguous, so every subtree is a range [lo, hi).
     * Invariant: keys[0..i) and _values[0..i) are sorted,
     * and a key and its value always share the same index */
    for (uint32_t i = 0; i < N; ++i) {
        /* The pair to insert */
        const char* const key = entries[i].Key();

        /* Shift every greater key one slot to the right */
        uint32_t j = i;
        while (j > 0 && String::Compare(keys[j - 1], key) > 0) {
            keys[j] = keys[j - 1];
            values[j] = values[j - 1];
            --j;
        }

        /* Put the pair into the freed slot */
        keys[j] = key;
        values[j] = entries[i].Value();
    }

    /* ---------- Step 2. Building the tree ---------- */

    /* An array to store the nodes to make
     * The static array from it in the future */
    Node nodes[2 * N] { };
    uint32_t nodes_num = 1;

    /* A node waiting to be built. Lives only during the construction */
    struct Task {
        uint32_t node_index = 0;
        uint32_t lo = 0;
        uint32_t hi = 0;
        uint32_t depth = 0;
    };

    /* The queue of nodes to build. Capacity N is enough: queued nodes
     * are never ancestors of each other, so their key ranges
     * are disjoint and non-empty */
    Queue<Task, N> queue;
    queue.Push(Task{ 0, 0, N, 0 });

    /* BFS: take a node, build it, push its children to the queue.
     * Ends when there are no unbuilt nodes left */
    while (!queue.IsEmpty()) {
        /* Take the next node and unpack its task */
        const Task task = queue.Pop();
        Node& node = nodes[task.node_index];

        /* Get the values from the task */
        uint32_t lo = task.lo;
        const uint32_t hi = task.hi;
        const uint32_t depth = task.depth;

        /* --- Terminal check --- */

        /* All the keys of the range match in the first d chars.
         * If a key has '\0' at position d, it ends exactly in this node.
         * Such a key is the shortest one, so it is the first in the range */
        if (keys[lo][depth] == '\0') {
            node.is_terminal = true;
            node.value_index = lo++;  // Take the key out of the range
            /* Two equal keys would both have '\0' at position d */
            assert((lo == hi || keys[lo][depth] != '\0') && "Duplicate key");
        }

        /* --- Leaf check --- */

        /* No keys left: nothing to split, the node has no children */
        if (lo == hi) continue;

        /* --- Children creation --- */

        /* All the children are created in the loop below without nested
         * processing, so they are contiguous in _nodes */
        node.first_child = nodes_num;
        uint32_t groups = 0;

        /* One iteration = one group of keys = one child */
        while (lo < hi) {
            /* Find the end of the group: keys with the same char
             * at position d. The keys are sorted, so the group is
             * the contiguous range [l, end) */
            uint32_t end = lo + 1;
            while (end < hi && keys[end][depth] == keys[lo][depth]) ++end;

            /* Compute the label: the common prefix of the group.
             * For sorted strings it equals the common prefix of the first
             * and the last one. The first chars match by the definition
             * of the group, so the length is at least 1 */
            const char* const string1 = keys[lo] + depth;
            const char* const string2 = keys[end - 1] + depth;
            uint32_t len = 1;
            while (string1[len] != '\0' && string1[len] == string2[len]) ++len;

            /* Create the child with its label. Its own subtree is deferred:
             * the task goes to the queue with the depth increased
             * by the label length */
            nodes[nodes_num].label =
                std::define_static_string(std::span(string1, len));
            queue.Push(Task{ nodes_num, lo, end, depth + len });

            /* Update the values */
            ++nodes_num;
            ++groups;

            /* Go to the next group */
            lo = end;
        }

        /* The number of the children is known only now */
        node.childs_num = (uint16_t)groups;
    }

    /* Save static arrays of Nodes and Values */
    _values = std::define_static_array(values).data();
    _nodes  = std::define_static_array(std::span(nodes, nodes_num)).data();
}

template <typename T>
template <RadixTree<T>::NextCharFunc FNextChar>
inline constexpr const T* RadixTree<T>::Get(const char* key) const noexcept {
    /* A null key can never match anything */
    assert(key != nullptr && "nullptr key can't match anything");

    /* Start from the root. It has an empty label and consumes no chars */
    const Node* node = _nodes;
    char key_chr = FNextChar(key);

    /* Descend one node per iteration, consuming the label of the node
     * we go into. Ends when the key is fully consumed or the path breaks */
    for (;;) {
        /* --- Key end check --- */

        /* The whole key has been consumed. It exists in the tree
         * only if the current node is terminal */
        if (key_chr == '\0')
            return node->is_terminal ? &_values[node->value_index] : nullptr;

        /* --- Child search --- */

        /* The children are contiguous in _nodes and sorted by the first
         * char of their labels (compared as unsigned, same as in StrCmp).
         * All the first chars are distinct, so a binary search is enough */
        const Node* const children = _nodes + node->first_child;
        const unsigned char chr = (unsigned char)key_chr;
        const Node* next = nullptr;

        uint32_t left = 0;
        uint32_t right = node->childs_num;
        while (left < right) {
            const uint32_t mid = left + (right - left) / 2;
            const unsigned char mid_chr = (unsigned char)*children[mid].label;

            /* Found the child that starts with the current char */
            if (mid_chr == chr) {
                next = &children[mid];
                break;
            }

            /* Narrow the search range */
            if (mid_chr < chr) left = mid + 1;
            else right = mid;
        }

        /* No child starts with this char (or the node is a leaf) */
        if (next == nullptr) return nullptr;

        /* --- Label match --- */

        /* The label must be fully contained in the rest of the key.
         * The first char is already known to match, but it is cheaper
         * to compare the whole label in one loop. If the key ends earlier,
         * '\0' will not match a label char, so the loop stops with nullptr */
        const char* label = next->label;
        while (*label != '\0') {
            if (*label != key_chr) return nullptr;
            ++label;
            key_chr = FNextChar(key);
        }

        /* --- Descend --- */

        /* The label is consumed, continue from the child */
        node = next;
    }
}

template <typename T>
template <RadixTree<T>::NextCharFunc FNextChar>
inline constexpr const T* RadixTree<T>::Get(
    const char* key, uint32_t len
) const noexcept {
    assert(key != nullptr && "nullptr key can't match anything");

    const Node* node = _nodes;
    char key_chr = len > 0 ? FNextChar(key) : '\0';

    for (;;) {
        /* --- Key end check --- */

        if (len == 0 || key_chr == '\0')
            return node->is_terminal ? &_values[node->value_index] : nullptr;

        /* --- Child search --- */

        const Node* const children = _nodes + node->first_child;
        const unsigned char chr = (unsigned char)key_chr;
        const Node* next = nullptr;

        uint32_t left = 0;
        uint32_t right = node->childs_num;

        while (left < right) {
            const uint32_t mid = left + (right - left) / 2;
            const unsigned char mid_chr =
                (unsigned char)*children[mid].label;

            if (mid_chr == chr) {
                next = &children[mid];
                break;
            }

            if (mid_chr < chr) left = mid + 1;
            else right = mid;
        }

        if (next == nullptr) return nullptr;

        /* --- Label match --- */

        const char* label = next->label;

        while (*label != '\0') {
            if (len == 0 || key_chr == '\0' || *label != key_chr)
                return nullptr;

            ++label;
            --len;

            if (len > 0) key_chr = FNextChar(key);
        }

        /* --- Descend --- */

        node = next;
    }
}
