#include "playlist.h"

#include "track/track.h"

Playlist::~Playlist() {
    while (_head != nullptr) CutNode(_head);
}

bool Playlist::IsEmpty() const {
    return _head == nullptr;
}

void Playlist::FreeNode(Node* const node) {
    delete node->value;
    delete node;
}

