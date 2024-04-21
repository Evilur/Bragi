#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include "converter/audio_to_opus.h"

#include <string>

class Track {
public:
	virtual ~Track();

	std::string GetTitle();
	std::string GetAlbum();
	std::string GetArtist();
	
	bool IsEnd() const;
	
	virtual int GetOpus(unsigned char *out) = 0;
protected:
	AudioToOpus* _converter;
	std::string _title;
	std::string _album;
	std::string _artist;
	
	bool _is_end = false;
};

#endif
