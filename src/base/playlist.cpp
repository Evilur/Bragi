#include "playlist.h"

#include "track/track.h"

bool Playlist::IsEmpty() const {
    return _head == nullptr;
}

void Playlist::DeleteNode(Node*& node) {
    const Node* const node_for_delete = node;
    node = node->next;
    delete node_for_delete->value;
    delete node_for_delete;
}

