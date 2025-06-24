#pragma once

#include "linked_list.hpp"

/**
 * Simple minimalistic implementation of hash map
 * @author Evilur <mail@evilur.com>
 * @since v2.0.0
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
     * @return Element with the similar key; nullptr if there is no such element
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
    /* Calculate the hash */
    u_int hash = GetHash(key);

    /* Put the node to the one of the linked lists */
    _lists[hash].Push({ key, element });
}

template<typename K, typename T>
T HashMap<K, T>::Get(K key) const {
    /* Calculate the hash */
    u_int hash = GetHash(key);

    /* Try to get the node from the linked list */
    for (Node node : _lists[hash])
        if (node.key == key)
            return node.element;

    /* If there is NOT an element in the linked list, return nullptr */
    return nullptr;
}