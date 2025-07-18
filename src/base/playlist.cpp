#include "playlist.h"

#include "track/track.h"

bool Playlist::IsEmpty() const {
    return _head == nullptr;
}

unsigned int Playlist::GetSize() const { return _size; }

void Playlist::Push(Track* element) {
    _size++;
    LinkedList::Push(element);
}

void Playlist::Pop() {
    _size--;
    LinkedList::Pop();
}

void Playlist::Pop(const unsigned int count) {
    _size -= count;
    LinkedList::Pop(count);
}

void Playlist::Remove(const unsigned int index, const unsigned int count) {
    _size -= count;
    LinkedList::Remove(index, count);
}

void Playlist::DeleteNode(Node*& node) {
    const Node* const node_for_delete = node;
    node = node->next;
    delete node_for_delete->value;
    delete node_for_delete;
}

