#include "track.h"
#include "util/dictionary.h"
#include "util/color.h"

Track::~Track() {
	delete _converter;
	_converter = nullptr;
}

std::string Track::GetFormattedDuration(const unsigned short &duration) {
	/* Get hours, minutes and seconds */
	const unsigned short seconds = duration % 60;
	const unsigned short minutes = (duration / 60) % 60;
	const unsigned short hours = duration / (60 * 60);

	const auto format = [](const unsigned short time) {
		char time_str[] = "00";
		if (time < 10) time_str[1] = time + '0';
		else {
			time_str[0] = (time / 10) + '0';
			time_str[1] = (time % 10) + '0';
		}
		return std::string(time_str);
	};

	if (hours > 0) return std::format(DIC_TRACK_MORE_THAN_HOUR, hours, format(minutes), format(seconds));
	else if (minutes > 0) return std::format(DIC_TRACK_LESS_THAN_HOUR, minutes, format(seconds));
	else return std::format(DIC_TRACK_LESS_THAN_MINUTE, seconds);
}