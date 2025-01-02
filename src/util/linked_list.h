#ifndef BRAGI_LINKED_LIST_H
#define BRAGI_LINKED_LIST_H

#include <iostream>

template<typename T>
class LinkedList final {
	struct Iterator;
	struct Node;
public:
	~LinkedList();

	void Append(T element);

	void PopFront();

	void PopFront(unsigned int count);

	void Remove(unsigned int index, unsigned int count = 1);

	T &operator[](unsigned int index) const;

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