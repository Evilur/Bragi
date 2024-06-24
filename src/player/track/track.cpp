#include "track.h"
#include "util/dictionary.h"
#include "util/color.h"

Track::~Track() {
	delete _converter;
	_converter = nullptr;
}

const std::string &Track::GetTitle() const { return _title; }

const unsigned short &Track::GetDuration() const { return _duration; }

Track::Track(const std::string &title, const unsigned short &duration) : _duration(duration), _title(title) { }