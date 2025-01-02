#ifndef BRAGI_LINKED_LIST_H
#define BRAGI_LINKED_LIST_H

#include <iostream>

template<typename T>
class LinkedList final {
	struct Iterator;
	struct Node;
public:
	typedef void(*FreeElement)(T);

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

#include "linked_list.tpp"

#endif