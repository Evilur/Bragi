#pragma once

#include "linked_list.h"
#include "util/class.h"
#include "util/equal.h"
#include "util/hash.h"

#include <cstdint>

template <typename T>
using dictionary_optimal_param =
    std::conditional_t<std::is_fundamental_v<T>, T, const T&>;

template <typename T>
using dictionary_hash_func = uint64_t(*)(
    std::conditional_t<std::is_integral_v<T>, T, const T&>
);

template <typename T>
using dictionary_equal = bool(*)(const T&, const T&);

/**
 * A simple implementation of a hash map
 * @author Evilur <the.evilur@gmail.com>
 * @tparam K Key typename
 * @tparam T Element typename
 * @tparam S Size typename
 * @tparam H Hash function
 * @tparam E Equal function
 */
template <typename K, typename T,
typename S = uint16_t,
dictionary_hash_func<K> H = ::hash,
dictionary_equal<K> E = ::equal>
class Dictionary final {
    struct Node;
public:
    PREVENT_COPY_ALLOW_MOVE(Dictionary);

    /**
     * @param capacity Maximum number of elements without resizing
     */
    explicit Dictionary(S capacity) noexcept;

    /**
     * Free the memory
     */
    ~Dictionary() noexcept;

    /**
     * Put an element into the hash map
     * @param key The key that can be used to retrieve the element
     * @param element Element to put into the map
     */
    bool Put(dictionary_optimal_param<K> key, const T& element) noexcept;

    /**
     * Put an element into the hash map
     * @param key The key that can be used to retrieve the element
     * @param element Element to put into the map
     */
    bool Put(dictionary_optimal_param<K> key, T&& element) noexcept;

    /**
     * Get the element from the hash map by the key
     * @param key The key to get the element by
     * @return Element with such a key, nullptr if there is no such element
     */
    T* Get(dictionary_optimal_param<K> key) noexcept;

    /**
     * Get the element from the hash map by the key
     * @param key The key to get the element by
     * @return Element with such a key, nullptr if there is no such element
     */
    const T* Get(dictionary_optimal_param<K> key) const noexcept;

    /**
     * Check the existence of the element in the hash map
     * @param key The key to check the element by
     * @return true if the element exists, false otherwise
     */
    bool Has(dictionary_optimal_param<K> key) const noexcept;

    /**
     * Delete the element from the hash map
     * @param key The key to delete the element by
     */
    bool Delete(dictionary_optimal_param<K> key) noexcept;

    /**
     * Iterator to go through the hash map
     */
    class Iterator {
    public:
        explicit Iterator(S index,
                          S capacity,
                          const LinkedList<Node>* lists,
                          LinkedList<Node>::Iterator iterator) noexcept;

        bool operator!=(const Iterator& other) const noexcept;

        Node& operator*() noexcept;

        const Node& operator*() const noexcept;

        Iterator& operator++() noexcept;

    private:
        /**
         * Index of the current bucket in the hash table
         */
        S _index;
        /**
         * Total number of buckets in the hash table
         */
        const S _capacity;
        /**
         * Pointer to the array of linked lists (buckets)
         * that make up the hash table
         */
        const LinkedList<Node>* const _buckets;
        /**
         * Iterator pointing to the current node
         * inside the current bucket's linked list
         */
        LinkedList<Node>::Iterator _iterator;
    };

    /**
     * Get the iterator for the first element
     * @return iterator for the first element
     */
    Iterator begin() const noexcept;

    /**
     * Get the iterator after the last element
     * @return iterator after the last element
     */
    Iterator end() const noexcept;

private:
    /**
     * Node to keep the element and its key in the linked list
     */
    struct Node final {
        K key;
        T element;
    };

    /**
     * A pointer to the dynamic array with
     * linked lists for resolving collisions
     */
    LinkedList<Node>* const _buckets;

    /**
     * A size of the array with linked lists
     */
    const S _capacity;
};

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
Dictionary<K, T, S, H, E>::Dictionary(const S capacity) noexcept :
        _buckets(new LinkedList<Node>[capacity]), _capacity(capacity) { }

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
Dictionary<K, T, S, H, E>::~Dictionary() noexcept { delete[] _buckets; }

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
bool Dictionary<K, T, S, H, E>::Put(dictionary_optimal_param<K> key,
                                    const T& element) noexcept {
    /* Calculate the key hash */
    const S hash = H(key) % _capacity;

    /* Try to get the node from the linked list */
    for (const Node& node : _buckets[hash])
        if (E(node.key, key))
            return false;

    /* If there is no an element with such a key yet,
     * put the node to the one of the buckets, according to the hash */
    _buckets[hash].Push({ key, element });
    return true;
}

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
bool Dictionary<K, T, S, H, E>::Put(dictionary_optimal_param<K> key,
                                    T&& element) noexcept {
    /* Calculate the key hash */
    const S hash = H(key) % _capacity;

    /* Try to get the node from the linked list */
    for (const Node& node : _buckets[hash])
        if (E(node.key, key))
            return false;

    /* If there is no an element with such a key yet,
     * put the node to the one of the buckets, according to the hash */
    _buckets[hash].Push({ key, std::move(element) });
    return true;
}

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
T* Dictionary<K, T, S, H, E>::Get(dictionary_optimal_param<K> key) noexcept {
    /* Calculate the key hash */
    const S hash = H(key) % _capacity;

    /* Try to get the node from the linked list */
    for (Node& node : _buckets[hash])
        if (E(node.key, key))
            return &node.element;

    /* If there is NOT an element in the linked list, return nullptr */
    return nullptr;
}

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
const T* Dictionary<K, T, S, H, E>::Get(dictionary_optimal_param<K> key)
const noexcept {
    /* Calculate the key hash */
    const S hash = H(key) % _capacity;

    /* Try to get the node from the linked list */
    for (const Node& node : _buckets[hash])
        if (E(node.key, key))
            return &node.element;

    /* If there is NOT an element in the linked list, return nullptr */
    return nullptr;
}

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
bool Dictionary<K, T, S, H, E>::Has(dictionary_optimal_param<K> key)
const noexcept {
    /* Calculate the key hash */
    const S hash = H(key) % _capacity;

    /* Try to get the node from the linked list */
    for (const Node& node : _buckets[hash])
        if (E(node.key, key))
            return true;

    /* If there is NOT an element in the linked list, return false */
    return false;
}

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
bool Dictionary<K, T, S, H, E>::Delete(dictionary_optimal_param<K> key)
noexcept {
    /* Calculate the key hash */
    const S hash = H(key) % _capacity;

    /* Try to get the node from the linked list */
    S index = 0;
    bool has_element = false;
    for (const Node& node : _buckets[hash]) {
        if (E(node.key, key)) {
            has_element = true;
            break;
        }
        ++index;
    }

    /* Throw an error if there is no an element with such a key */
    if (!has_element) return false;

    /* If all is OK delete the element with such an index from the bucket */
    _buckets[hash].Remove(index);
    return true;
}

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
Dictionary<K, T, S, H, E>::Iterator Dictionary<K, T, S, H, E>::begin()
const noexcept {
    /* Iterate over all buckets */
    for (S i = 0; i < _capacity; ++i)
        /* If the current bucket is not empty,
         * return iterator to its first element */
        if (_buckets[i].begin() != _buckets[i].end())
            return Iterator(i, _capacity, _buckets, _buckets[i].begin());

    /* If all buckets are empty, return end() iterator */
    return end();
}

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
Dictionary<K, T, S, H, E>::Iterator Dictionary<K, T, S, H, E>::end()
const noexcept {
    /* Construct an iterator representing the end position:
     * index == capacity and internal list iterator == nullptr */
    return Iterator(_capacity, _capacity, _buckets,
                    typename LinkedList<Node>::Iterator(nullptr));
}

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE Dictionary<K, T, S, H, E>::Iterator::Iterator(
    const S index,
    const S capacity,
    const LinkedList<Node>* const lists,
    const typename LinkedList<Node>::Iterator iterator
) noexcept : _index(index), _capacity(capacity),
             _buckets(lists), _iterator(iterator) { }

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE
bool Dictionary<K, T, S, H, E>::Iterator::operator!=(const Iterator& other)
const noexcept {
    /* If both iterators are "end" iterators, they are equal */
    if (_index == _capacity && other._index == other._capacity) return false;

    /* If they point to different buckets, they are not equal */
    if (_index != other._index) return true;

    /* Otherwise compare underlying list iterators */
    return _iterator != other._iterator;
}

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE Dictionary<K, T, S, H, E>::Node&
Dictionary<K, T, S, H, E>::Iterator::operator*()
noexcept { return *_iterator; }

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE const Dictionary<K, T, S, H, E>::Node&
Dictionary<K, T, S, H, E>::Iterator::operator*()
const noexcept { return *_iterator; }

template <typename K, typename T, typename S,
dictionary_hash_func<K> H,
dictionary_equal<K> E>
FORCE_INLINE Dictionary<K, T, S, H, E>::Iterator&
Dictionary<K, T, S, H, E>::Iterator::operator++() noexcept {
    /* If iterator is already at end(), do nothing */
    if (_index == _capacity) return *this;

    /* If still inside a non-empty bucket, stop here */
    if (++_iterator != _buckets[_index].end()) return *this;

    /* Skip empty buckets until a non-empty one is found */
    while (++_index < _capacity) {
        if (_buckets[_index].begin() != _buckets[_index].end()) {
            _iterator = _buckets[_index].begin();
            return *this;
        }
    }

    /* If no more buckets contain elements, set iterator to end() */
    _index = _capacity;
    _iterator = typename LinkedList<Node>::Iterator(nullptr);
    return *this;
}
