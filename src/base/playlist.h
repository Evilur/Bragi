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
    bool IsEmpty() const;

    unsigned int GetSize() const;

    void Push(Track* element) override;

    void Pop() override;

    void Pop(unsigned int count) override;

    void Remove(unsigned int index, unsigned int count = 1) override;

private:
    unsigned int _size = 0;

    void DeleteNode(Node*& node) override;
};
