#pragma once

#include "types/base.h"
#include "util/linked_list.h"

/**
 * Simple minimalistic implementation of hash map
 * @author Evilur <mail@evilur.com>
 * @since v2.0.0-alpha.3
 * @tparam K Key typename
 * @tparam T Element typename
 */
template<typename K, typename T>
class HashMap {
public:
    /**
     * @param capacity Maximum number of elements without resizing
     */
    explicit HashMap(u_int capacity);

    virtual ~HashMap();

    /**
     * Put an element into the hash map
     * @param key The key that can be used to retrieve the element
     * @param element Element to put into the map
     */
    void Put(K key, T element);

    /**
     * Get the element from the hash map by the key
     * @param key The key to get the element by
     * @return Element with the similar key
     */
    T Get(K key) const;

protected:
    /**
     * A node to keep in the linked list
     */
    class Node {
    public:
        K key;
        T element;
    };

    /**
     * A pointer to the dynamic array with linked lists for resolving collisions
     */
    LinkedList<Node>* const _lists;
    /**
     * A size of the dynamic array with linked lists
     */
    const u_int _lists_size;

    /**
     * An abstract method for calculating the hash
     * @param key A key for calculating the hash of
     * @return Calculated hash
     */
    virtual u_int GetHash(K key) const = 0;
};

template<typename K, typename T>
HashMap<K, T>::HashMap(const u_int capacity) :
        _lists(new LinkedList<Node>[capacity]), _lists_size(capacity) { }

template <typename K, typename T>
HashMap<K, T>::~HashMap() { delete[] _lists; }

template<typename K, typename T>
void HashMap<K, T>::Put(K key, T element) {
    u_int hash = GetHash(key);
    _lists[hash].Push({ key, element });
}

template<typename K, typename T>
T HashMap<K, T>::Get(K key) const {
    u_int hash = GetHash(key);

    for (Node node : _lists[hash])
        if (node.key == key)
            return node.element;

    return nullptr;
}