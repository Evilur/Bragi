#include "track.h"

Track::~Track() { delete converter; }

std::string Track::GetTitle() { return title; }

std::string Track::GetAlbum() { return album; }

std::string Track::GetArtist() { return artist; }

bool Track::IsEnd() const { return is_end; }
