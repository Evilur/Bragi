#ifndef BRAGI_LINKED_LIST_H
#define BRAGI_LINKED_LIST_H

#include <iostream>

template<typename T>
class LinkedList final {
        struct Iterator;
        struct Node;
public:
        using FreeElement = void(*)(T);

        ~LinkedList();

        T &Head();

        void Push(T element);

        void PopFront(const FreeElement free_element = FreeElementPlaceholder);

        void PopFront(unsigned int count, const FreeElement free_element = FreeElementPlaceholder);

        void Remove(unsigned int index, unsigned int count = 1, const FreeElement free_element = FreeElementPlaceholder);

        T &operator[](unsigned int index) const;

        Iterator begin() const;

        Iterator end() const;

private:
        struct Node {
                Node(const T &value);

                T value;
                Node* next = nullptr;
        };

        struct Iterator {
                Iterator(LinkedList<T>::Node* node_ptr);

                bool operator!=(const Iterator &other);

                const T &operator*() const;

                Iterator &operator++();

        private:
                Node* _node;
        };

        Node* _head = nullptr;
        Node* _tail = nullptr;

        inline static void FreeElementPlaceholder(T);
};

template<typename T>
LinkedList<T>::~LinkedList() {
        while (_head != nullptr) {
                Node* temp_node = _head;
                _head = _head->next;
                delete temp_node;
        }
}

template<typename T>
T &LinkedList<T>::Head() { return _head->value; }

template<typename T>
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

template<typename T>
void LinkedList<T>::PopFront(const FreeElement free_element) {
        if (_head == nullptr) throw std::runtime_error("LinkedList: PopFront() index out of range");
        Node* unnes_node = _head;
        _head = _head->next;
        free_element(unnes_node->value);
        delete unnes_node;
}

template<typename T>
void LinkedList<T>::PopFront(unsigned int count, const FreeElement free_element) {
        while (count-- > 0) {
                if (_head == nullptr) throw std::runtime_error("LinkedList: PopFront(unsigned int) index out of range");
                Node* unnes_node = _head;
                _head = _head->next;
                free_element(unnes_node->value);
                delete unnes_node;
        }
}

template<typename T>
void LinkedList<T>::Remove(unsigned int index, unsigned int count, const FreeElement free_element) {
        /* If we need to remove the first elements */
        if (index == 0) {
                PopFront(count);
                return;
        }

        /* Get the last node before the deletions */
        Node* node_before = _head;
        while (index-- > 1) {
                node_before = node_before->next;
                if (!node_before) throw std::runtime_error("LinkedList: Remove(unsigned int, unsigned int = 1) index out of range");
        }

        /* Get the first node after deletions, and delete others */
        Node* node_after = node_before->next;
        while (count-- > 0) {
                if (!node_after) throw std::runtime_error("LinkedList: Remove(unsigned int, unsigned int = 1) index out of range");
                Node* unnes_node = node_after;
                node_after = node_after->next;
                free_element(unnes_node->value);
                delete unnes_node;
        }

        /* Link the node before deletions and the node after deletions */
        node_before->next = node_after;
}

template<typename T>
T &LinkedList<T>::operator[](unsigned int index) const {
        Node* node_ptr = _head;
        while (index-- > 0) {
                node_ptr = node_ptr->next;
                if (!node_ptr) throw std::runtime_error("LinkedList: operator[](unsigned int) index out of range");
        }
        return node_ptr->value;
}

template<typename T>
LinkedList<T>::Iterator LinkedList<T>::begin() const { return Iterator(_head); }

template<typename T>
LinkedList<T>::Iterator LinkedList<T>::end() const { return nullptr; }

template<typename T>
LinkedList<T>::Node::Node(const T &value) : value(value) { }

template<typename T>
LinkedList<T>::Iterator::Iterator(LinkedList::Node* node_ptr) : _node(node_ptr) { }

template<typename T>
bool LinkedList<T>::Iterator::operator!=(const LinkedList::Iterator &other) {
        return _node != other._node;
}

template<typename T>
const T &LinkedList<T>::Iterator::operator*() const {
        return _node->value;
}

template<typename T>
LinkedList<T>::Iterator &LinkedList<T>::Iterator::operator++() {
        _node = _node->next;
        return *this;
}

template<typename T>
inline void LinkedList<T>::FreeElementPlaceholder(T) { }


#endif