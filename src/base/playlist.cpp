#include "playlist.h"

#include "track/track.h"

Playlist::~Playlist() noexcept {
    while (_head != nullptr) CutNode(_head);
}

bool Playlist::IsEmpty() const noexcept {
    return _head == nullptr;
}

void Playlist::FreeNode(Node* const node) const noexcept {
    delete node->value;
    delete node;
}

