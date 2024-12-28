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
template<typename IterFunc>
void LinkedList<T>::Iterate(IterFunc func) {
	Node* node_ptr = _head;
	while (node_ptr != nullptr) {
		func(node_ptr->value);
		node_ptr = node_ptr->next;
	}
}