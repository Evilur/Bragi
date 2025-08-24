#pragma once

#include <stdexcept>

template <typename T>
class LinkedList {
protected:
    class Iterator;
    class Node;

public:
    virtual ~LinkedList() noexcept;

    T& Head() const noexcept;

    T& Tail() const noexcept;

    void Push(T element) noexcept;

    void Pop();

    bool TryPop() noexcept;

    void PopTail();

    bool TryPopTail() noexcept;

    void Pop(unsigned int count);

    unsigned int TryPop(unsigned int count) noexcept;

    T& operator[](unsigned int index) const;

    Iterator begin() const noexcept;

    Iterator end() const noexcept;

protected:
    class Node {
    public:
        explicit Node(const T& value) noexcept;

        T value;
        Node* next = nullptr;
    };

    class Iterator {
    public:
        explicit Iterator(Node* node_ptr) noexcept;

        bool operator!=(const Iterator& other) noexcept;

        const T& operator*() const noexcept;

        Iterator& operator++() noexcept;

    private:
        Node* _node;
    };

    Node* _head = nullptr;
    Node* _tail = nullptr;

    void CutNode(Node*& node) noexcept;

    virtual void FreeNode(Node* node) const noexcept;
};

template <typename T>
LinkedList<T>::~LinkedList() noexcept {
    while (_head != nullptr) CutNode(_head);
}

template <typename T>
T& LinkedList<T>::Head() const noexcept { return _head->value; }

template <typename T>
T& LinkedList<T>::Tail() const noexcept { return _tail->value; }

template <typename T>
void LinkedList<T>::Push(T element) noexcept {
    /* If the list was empty */
    if (!_head) {
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
void LinkedList<T>::Pop() {
    if (_head == nullptr)
        throw std::runtime_error("LinkedList: Pop() index out of range");
    CutNode(_head);
    if (_head == nullptr) _tail = nullptr;
}

template <typename T>
bool LinkedList<T>::TryPop() noexcept {
    if (_head == nullptr) return false;
    CutNode(_head);
    if (_head == nullptr) _tail = nullptr;
    return true;
}

template <typename T>
void LinkedList<T>::PopTail() {
    if (_tail == nullptr)
        throw std::runtime_error("LinkedList: PopTail() index out of range");
    CutNode(_tail);
    if (_tail == nullptr) _head = nullptr;
}

template <typename T>
bool LinkedList<T>::TryPopTail() noexcept {
    if (_tail == nullptr) return false;
    CutNode(_tail);
    if (_tail == nullptr) _head = nullptr;
    return true;
}

template <typename T>
void LinkedList<T>::Pop(unsigned int count) {
    while (count-- > 0) {
        if (_head == nullptr)
            throw std::runtime_error(
                "LinkedList: Pop(unsigned int) index out of range");
        CutNode(_head);
    }
    if (_head == nullptr) _tail = nullptr;
}

template <typename T>
unsigned int LinkedList<T>::TryPop(const unsigned int count) noexcept {
    for (unsigned int i = 0; i < count; i++) {
        if (_head == nullptr) return i;
        CutNode(_head);
    }
    if (_head == nullptr) _tail = nullptr;
    return count;
}

template <typename T>
T& LinkedList<T>::operator[](unsigned int index) const {
    Node* node_ptr = _head;
    while (index-- > 0) {
        node_ptr = node_ptr->next;
        if (!node_ptr)
            throw std::runtime_error(
                "LinkedList: operator[](unsigned int) index out of range");
    }
    return node_ptr->value;
}

template <typename T>
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
LinkedList<T>::Iterator::Iterator(Node* node_ptr) noexcept : _node(node_ptr) { }

template <typename T>
bool LinkedList<T>::Iterator::operator!=(const Iterator& other) noexcept {
    return _node != other._node;
}

template <typename T>
const T& LinkedList<T>::Iterator::operator*() const noexcept {
    return _node->value;
}

template <typename T>
LinkedList<T>::Iterator& LinkedList<T>::Iterator::operator++() noexcept {
    _node = _node->next;
    return *this;
}

template <typename T>
void LinkedList<T>::CutNode(Node*& node) noexcept {
    Node* const next_node = node->next;
    FreeNode(node);
    node = next_node;
}

template <typename T>
void LinkedList<T>::FreeNode(Node* const node) const noexcept {
    delete node;
}