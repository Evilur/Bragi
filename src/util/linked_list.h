#ifndef BRAGI_LINKED_LIST_H
#define BRAGI_LINKED_LIST_H

#include <iostream>

template<typename T>
class LinkedList final {
	struct Iterator;
	struct Node;
public:
	void Append(T element);

	Iterator begin() const;

	Iterator end() const;

private:
	struct Node {
		Node(T value);

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
};

#include "linked_list.tpp"

#endif