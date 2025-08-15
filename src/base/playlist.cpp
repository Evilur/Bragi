#include "playlist.h"

#include "track/track.h"

Playlist::~Playlist() noexcept {
    while (_head) CutNode(_head);
}

bool Playlist::IsEmpty() const noexcept {
    return _head == nullptr;
}

void Playlist::Loop() noexcept {
    /* Move the current track to the end of the list */
    _tail->next = _head;
    _tail = _tail->next;

    /* Cut the old first element */
    _head = _head->next;
    _tail->next = nullptr;
}

void Playlist::FreeNode(Node* const node) const noexcept {
    delete node->value;
    delete node;
}

