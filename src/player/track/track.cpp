#include "track.h"
#include "util/dictionary.h"
#include "util/color.h"

void Track::Init() {
	if (!_initted) {
		OnInit();
		_initted = true;
	}
}

Track::~Track() {
	delete _converter;
	_converter = nullptr;
}

const unsigned short &Track::GetDuration() const { return _duration; }

Track::Track(const unsigned short &duration) : _duration(duration) { }