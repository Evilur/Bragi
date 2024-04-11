#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include <string>
#include "converter/opus_converter.h"

class Track {
public:
	virtual ~Track();

	std::string GetTitle();
	std::string GetAlbum();
	std::string GetArtist();
	
	bool IsEnd() const;
	
	virtual int GetOpus(unsigned char *out) = 0;
protected:
	OpusConverter* converter;
	std::string title;
	std::string album;
	std::string artist;
	
	bool is_end = false;
};

#endif
