#include "track.h"

Track::~Track() { 
	delete _converter;
	_converter = nullptr;
}

std::string Track::GetTitle() { return _title; }

std::string Track::GetAlbum() { return _album; }

std::string Track::GetArtist() { return _artist; }

bool Track::IsEnd() const { return _is_end; }
