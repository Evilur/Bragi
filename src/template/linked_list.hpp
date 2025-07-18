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

    virtual void Push(T element);

    virtual void Pop();

    virtual void Pop(unsigned int count);

    virtual void Remove(unsigned int index, unsigned int count = 1);

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

    virtual void DeleteNode(Node*& node);
};

template <typename T>
LinkedList<T>::~LinkedList() {
    while (_head != nullptr) {
        LinkedList::DeleteNode(_head);
    }
}

template <typename T>
T& LinkedList<T>::Head() { return _head->value; }

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
    DeleteNode(_head);
}

template <typename T>
void LinkedList<T>::Pop(unsigned int count) {
    while (count-- > 0) {
        if (_head == nullptr)
            throw std::runtime_error(
                "LinkedList: Pop(unsigned int) index out of range");
        DeleteNode(_head);
    }
}

template <typename T>
void LinkedList<T>::Remove(unsigned int index, unsigned int count) {
    /* If we need to remove the first elements */
    if (index == 0) {
        Pop(count);
        return;
    }

    /* Get the last node before the deletions */
    Node* node_before = _head;
    while (index-- > 1) {
        node_before = node_before->next;
        if (!node_before)
            throw std::runtime_error(
                "LinkedList: Remove(unsigned int, unsigned int = 1) index out of range");
    }

    /* Get the first node after deletions, and delete others */
    Node* node_after = node_before->next;
    while (count-- > 0) {
        if (!node_after)
            throw std::runtime_error(
                "LinkedList: Remove(unsigned int, unsigned int = 1) index out of range");
        DeleteNode(node_after);
    }

    /* Link the node before deletions and the node after deletions */
    node_before->next = node_after;
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
void LinkedList<T>::DeleteNode(Node*& node) {
    const Node* const node_for_delete = node;
    node = node->next;
    delete node_for_delete;
}

#endif