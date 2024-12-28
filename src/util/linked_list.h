#ifndef BRAGI_LINKED_LIST_H
#define BRAGI_LINKED_LIST_H

#include <iostream>

template<typename T>
class LinkedList final {
public:
	void Append(T element);

	template<typename IterFunc>
	void Iterate(IterFunc func) {
		Node* node_ptr = _head;
		while (node_ptr != nullptr) {
			func(node_ptr->value);
			node_ptr = node_ptr->next;
		}
	}

private:
	struct Node {
		Node(T value) : value(value) { };
		T value;
		Node* next = nullptr;
	};

	Node* _head = nullptr;
	Node* _tail = nullptr;
};

#endif