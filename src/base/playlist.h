#pragma once

#include "template/linked_list.hpp"

class Track;

/**
 * A container for tracks to play
 * @note {
 * Exists for the entire lifetime of the program,
 * so there is no need to implement a destructor
 * }
 */
class Playlist final : public LinkedList<Track*> {
public:
    bool IsEmpty() const noexcept;

    void Loop() noexcept;

private:
    unsigned int _size = 0;

    void FreeNode(Node* node) const noexcept override;
};
