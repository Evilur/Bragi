#pragma once

#include "template/linked_list.hpp"

class Track;

class Playlist final : public LinkedList<Track*> {
public:
    virtual ~Playlist() noexcept;

    bool IsEmpty() const noexcept;

    void Loop() noexcept;

private:
    unsigned int _size = 0;

    void FreeNode(Node* node) const noexcept override;
};
