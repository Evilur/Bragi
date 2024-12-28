template<typename T>
void LinkedList<T>::Append(T element) {
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
LinkedList<T>::Iterator LinkedList<T>::begin() const { return Iterator(_head); }

template<typename T>
LinkedList<T>::Iterator LinkedList<T>::end() const { return nullptr; }

template<typename T>
LinkedList<T>::Node::Node(T value) : value(value) { }

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