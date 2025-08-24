#pragma once

#include "template/linked_list.hpp"

class Track;

class Playlist final : public LinkedList<Track*> {
public:
    ~Playlist() noexcept override;

    bool IsEmpty() const noexcept;

    void Loop() noexcept;

private:
    void FreeNode(Node* node) const noexcept override;
};
