#pragma once

#include "util/class.h"
#include "util/macro.h"

#include <algorithm>
#include <cstdint>

template <typename T>
class LinkedList final {
private:
    struct Node;

public:
    PREVENT_COPY_ALLOW_MOVE(LinkedList);

    LinkedList() = default;

    virtual ~LinkedList() noexcept;

    T* Head() noexcept;

    const T* Head() const noexcept;

    T* Tail() noexcept;

    const T* Tail() const noexcept;

    void Push(const T& element) noexcept;

    void Push(T&& element) noexcept;

    bool Pop() noexcept;

    uint32_t Pop(uint32_t number) noexcept;

    bool PopTail() noexcept;

    uint32_t Remove(uint32_t index, uint32_t number = 1) noexcept;

    T* Get(uint32_t index) noexcept;

    const T* Get(uint32_t index) const noexcept;

    class Iterator {
    public:
        explicit Iterator(Node* node_ptr) noexcept;

        bool operator!=(const Iterator& other) const noexcept;

        T& operator*() noexcept;

        const T& operator*() const noexcept;

        Iterator& operator++() noexcept;

    private:
        Node* _node;
    };

    Iterator begin() const noexcept;

    Iterator end() const noexcept;

private:
    struct Node final {
        explicit Node(const T& value) noexcept;

        explicit Node(T&& value) noexcept;

        T value;
        Node* next = nullptr;
    };

    Node* _head = nullptr;
    Node* _tail = nullptr;

    void CutNode(Node*& node) noexcept;
};

template <typename T>
FORCE_INLINE
LinkedList<T>::~LinkedList() noexcept {
    while (_head != nullptr) CutNode(_head);
}

template <typename T>
FORCE_INLINE
T* LinkedList<T>::Head() noexcept {
    return _head == nullptr ? nullptr : &_head->value;
}

template <typename T>
FORCE_INLINE
const T* LinkedList<T>::Head() const noexcept {
    return _head == nullptr ? nullptr : &_head->value;
}

template <typename T>
FORCE_INLINE
T* LinkedList<T>::Tail() noexcept {
    return _tail == nullptr ? nullptr : &_tail->value;
}

template <typename T>
FORCE_INLINE
const T* LinkedList<T>::Tail() const noexcept {
    return _tail == nullptr ? nullptr : &_tail->value;
}

template <typename T>
FORCE_INLINE
void LinkedList<T>::Push(const T& element) noexcept {
    /* If the list was empty */
    if (_head == nullptr) {
        _head = new Node(element);
        _tail = _head;
        return;
    }

    /* If the list was NOT empty, add a pointer of the new node to the last one
     * and replace the last node with the new one */
    _tail->next = new Node(element);
    _tail = _tail->next;
}

template <typename T>
FORCE_INLINE
void LinkedList<T>::Push(T&& element) noexcept {
    /* If the list was empty */
    if (_head == nullptr) {
        _head = new Node(std::move(element));
        _tail = _head;
        return;
    }

    /* If the list was NOT empty, add a pointer of the new node to the last one
     * and replace the last node with the new one */
    _tail->next = new Node(std::move(element));
    _tail = _tail->next;
}

template <typename T>
FORCE_INLINE
bool LinkedList<T>::Pop() noexcept {
    if (_head == nullptr) return false;
    CutNode(_head);
    if (_head == nullptr) _tail = nullptr;
    return true;
}

template <typename T>
FORCE_INLINE
uint32_t LinkedList<T>::Pop(const uint32_t number) noexcept {
    for (uint32_t i = 0; i < number; ++i) {
        if (_head == nullptr) {
            _tail = nullptr;
            return i;
        }
        CutNode(_head);
    }
    if (_head == nullptr) _tail = nullptr;
    return number;
}

template <typename T>
FORCE_INLINE
uint32_t LinkedList<T>::Remove(uint32_t index,
                                   uint32_t number) noexcept {
    /* If we are deleting first elements */
    if (index == 0) return Pop(number);

    /* Get the element before the removable */
    Node* before_removable = _head;
    while (index-- > 1) {
        if (before_removable == nullptr) return 0;
        before_removable = before_removable->next;
    }

    /* A variable for store the result */
    uint32_t result = 0;

    /* Remove elements */
    while (number-- > 0) {
        if ((before_removable == nullptr) || !before_removable->next)
            return result;
        CutNode(before_removable->next);
        ++result;

        /* If we get the last element, update the tail */
        if (!before_removable->next) _tail = before_removable;
    }

    /* If the head is nullptr, _tail must be nullptr too */
    if (_head == nullptr) _tail = nullptr;

    /* Return the result */
    return result;
}

template <typename T>
FORCE_INLINE
bool LinkedList<T>::PopTail() noexcept {
    /* If the list is empty */
    if (_head == nullptr) return false;

    /* If we have only one element */
    if (!_head->next) {
        CutNode(_head);
        _tail = nullptr;
        return true;
    }

    /* Get the second last node */
    Node* second_last_node = _head;
    while (second_last_node->next->next)
        second_last_node = second_last_node->next;

    /* Change the tail */
    CutNode(second_last_node->next);
    _tail = second_last_node;
    return true;
}

template <typename T>
FORCE_INLINE
T* LinkedList<T>::Get(uint32_t index) noexcept {
    Node* node_ptr = _head;
    while (index-- > 0) {
        node_ptr = node_ptr->next;
        if (node_ptr == nullptr) return nullptr;
    }
    return &node_ptr->value;
}

template <typename T>
FORCE_INLINE
const T* LinkedList<T>::Get(uint32_t index) const noexcept {
    Node* node_ptr = _head;
    while (index-- > 0) {
        node_ptr = node_ptr->next;
        if (node_ptr == nullptr) return nullptr;
    }
    return &node_ptr->value;

}

template <typename T>
FORCE_INLINE
LinkedList<T>::Iterator LinkedList<T>::begin() const noexcept {
    return Iterator(_head);
}

template <typename T>
LinkedList<T>::Iterator LinkedList<T>::end() const noexcept {
    return Iterator(nullptr);
}

template <typename T>
LinkedList<T>::Node::Node(const T& value) noexcept : value(value) { }

template <typename T>
LinkedList<T>::Node::Node(T&& value) noexcept : value(std::move(value)) { }

template <typename T>
LinkedList<T>::Iterator::Iterator(Node* node_ptr) noexcept :
    _node(node_ptr) { }

template <typename T>
bool
LinkedList<T>::Iterator::operator!=(const Iterator& other) const noexcept {
    return _node != other._node;
}

template <typename T>
T& LinkedList<T>::Iterator::operator*()
noexcept { return _node->value; }

template <typename T>
const T& LinkedList<T>::Iterator::operator*()
const noexcept { return _node->value; }

template <typename T>
LinkedList<T>::Iterator& LinkedList<T>::Iterator::operator++() noexcept {
    _node = _node->next;
    return *this;
}

template <typename T>
void LinkedList<T>::CutNode(Node*& node) noexcept {
    Node* const next_node = node->next;
    delete node;
    node = next_node;
}
