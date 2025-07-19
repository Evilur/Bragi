#ifndef BRAGI_LINKED_LIST_H
#define BRAGI_LINKED_LIST_H

#include <iostream>

template <typename T>
class LinkedList {
protected:
    struct Iterator;
    struct Node;

public:
    virtual ~LinkedList();

    T& Head();

    T& Tail();

    void Push(T element);

    void Pop();

    bool TryPop();

    void Pop(unsigned int count);

    unsigned int TryPop(unsigned int count);

    T& operator[](unsigned int index) const;

    Iterator begin() const;

    Iterator end() const;

protected:
    struct Node {
        Node(const T& value);

        T value;
        Node* next = nullptr;
    };

    struct Iterator {
        Iterator(Node* node_ptr);

        bool operator!=(const Iterator& other);

        const T& operator*() const;

        Iterator& operator++();

    private:
        Node* _node;
    };

    Node* _head = nullptr;
    Node* _tail = nullptr;

    void CutNode(Node*& node);

    virtual void FreeNode(Node* node);
};

template <typename T>
LinkedList<T>::~LinkedList() {
    while (_head != nullptr) CutNode(_head);
}

template <typename T>
T& LinkedList<T>::Head() { return _head->value; }

template <typename T>
T& LinkedList<T>::Tail() { return _tail->value; }

template <typename T>
void LinkedList<T>::Push(T element) {
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
}

template <typename T>
bool LinkedList<T>::TryPop() {
    if (_head == nullptr) return false;
    CutNode(_head);
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
}

template <typename T>
unsigned int LinkedList<T>::TryPop(const unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        if (_head == nullptr) return i;
        CutNode(_head);
    }
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
LinkedList<T>::Iterator LinkedList<T>::begin() const { return Iterator(_head); }

template <typename T>
LinkedList<T>::Iterator LinkedList<T>::end() const { return nullptr; }

template <typename T>
LinkedList<T>::Node::Node(const T& value) : value(value) {
}

template <typename T>
LinkedList<T>::Iterator::Iterator(Node* node_ptr) : _node(node_ptr) {
}

template <typename T>
bool LinkedList<T>::Iterator::operator!=(const Iterator& other) {
    return _node != other._node;
}

template <typename T>
const T& LinkedList<T>::Iterator::operator*() const {
    return _node->value;
}

template <typename T>
LinkedList<T>::Iterator& LinkedList<T>::Iterator::operator++() {
    _node = _node->next;
    return *this;
}

template <typename T>
void LinkedList<T>::CutNode(Node*& node) {
    Node* const next_node = node->next;
    FreeNode(node);
    node = next_node;
}

template <typename T>
void LinkedList<T>::FreeNode(Node* const node) {
    delete node;
}

#endif