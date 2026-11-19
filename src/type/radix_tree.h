#pragma once

#include "type/queue.h"
#include "util/class.h"
#include "type/key_value_pair.h"

#include <cstdint>
#include <cstdio>

namespace radix_detail {
constexpr int32_t StrCmp(const char* x1, const char* x2) noexcept;
} // namespace radix_detail

/**
 * A compile-time Radix (Compressed) Tree implementation
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T, uint32_t N>
class RadixTree final {
public:
    static_assert(N > 0, "RadixTrie must contain at least one entry");

    PREVENT_COPY_ALLOW_MOVE(RadixTree);

    consteval RadixTree(
        const KeyValuePair<const char*, T> (&entries)[N]
    ) noexcept;

    constexpr access_t<T> Get(const char* key) const noexcept;

private:
    struct Node {
        const char* label = nullptr;
        uint32_t label_len = 0;

        uint32_t first_child = 0;
        uint16_t childs_num = 0;

        bool is_terminal = false;
        uint32_t value_index = 0;
    };

    T _values[N];

    Node _nodes[2 * N] { };
    Node* _root = _nodes;
};

inline constexpr int32_t radix_detail::StrCmp(
    const char* x1, const char* x2
) noexcept {
    while (*x1 != '\0' && *x1 == *x2) { ++x1; ++x2; }
    return (uint8_t)*x1 - (uint8_t)*x2;
}

template <typename T, uint32_t N>
inline consteval RadixTree<T, N>::RadixTree(
    const KeyValuePair<const char*, T> (&entries)[N]
) noexcept {
    /* ---------- Step 1. Sorting ---------- */

    /* Sorted keys. Local: labels point into the string literals,
     * so the array is not needed after the construction */
    const char* keys[N]{};

    /* Insertion sort of the pairs by key. Keys with a common prefix
     * become contiguous, so every subtree is a range [lo, hi).
     * Invariant: keys[0..i) and _values[0..i) are sorted,
     * and a key and its value always share the same index */
    for (uint32_t i = 0; i < N; ++i) {
        /* The pair to insert */
        const char* const key = entries[i].Key();

        /* Shift every greater key one slot to the right */
        uint32_t j = i;
        while (j > 0 && radix_detail::StrCmp(keys[j - 1], key) > 0) {
            keys[j] = keys[j - 1];
            _values[j] = _values[j - 1];
            --j;
        }

        /* Put the pair into the freed slot */
        keys[j] = key;
        _values[j] = entries[i].Value();
    }

    /* ---------- Step 2. Building the tree ---------- */

    /* A node waiting to be built. Lives only during the construction */
    struct Task {
        uint32_t node_index = 0;
        uint32_t lo = 0;
        uint32_t hi = 0;
        uint32_t depth = 0;
    };

    /* The root: empty label, covers all the keys, nothing consumed.
     * Its Task is pushed below */
    _nodes[0].label = "";

    /* Number of created nodes */
    uint32_t count = 1;

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
        Node& node = _nodes[task.node_index];

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
        node.first_child = count;
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
            const char* const a = keys[lo] + depth;
            const char* const b = keys[end - 1] + depth;
            uint32_t len = 1;
            while (a[len] != '\0' && a[len] == b[len]) ++len;

            /* Create the child with its label. Its own subtree is deferred:
             * the task goes to the queue with the depth increased
             * by the label length */
            _nodes[count].label = a;
            _nodes[count].label_len = len;
            queue.Push(Task{ count, lo, end, depth + len });

            /* Update the values */
            ++count;
            ++groups;

            /* Go to the next group */
            lo = end;
        }

        /* The number of the children is known only now */
        node.childs_num = (uint16_t)groups;
    }
}

template <typename T, uint32_t N>
inline constexpr
access_t<T> RadixTree<T, N>::Get(
    const char* const key
) const noexcept {
        struct Frame {
        uint32_t node;
        uint32_t level;
        bool is_last;
    };

    Frame stack[2 * N] { };
    bool last[2 * N + 1] { };
    uint32_t top = 0;

    stack[top++] = Frame{ 0, 0, true };

    while (top > 0) {
        const Frame frame = stack[--top];
        const Node& node = _nodes[frame.node];

        last[frame.level] = frame.is_last;

        for (uint32_t i = 1; i < frame.level; ++i)
            printf("%s", last[i] ? "   " : "│  ");
        if (frame.level > 0)
            printf("%s", frame.is_last ? "└─ " : "├─ ");

        if (frame.level == 0) printf("(root)");
        else printf("\"%.*s\"", (int)node.label_len, node.label);
        if (node.is_terminal) printf(" [%u]", node.value_index);
        printf("\n");

        for (uint32_t i = node.childs_num; i > 0; --i)
            stack[top++] = Frame{
                node.first_child + i - 1,
                frame.level + 1,
                i == node.childs_num
            };
    }
    return 666;
}
