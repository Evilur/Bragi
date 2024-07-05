#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include "converter/audio_to_opus.h"

#include <dpp/dpp.h>
#include <string>

class Track {
public:
	virtual ~Track();

	void Init();

	const unsigned short &GetDuration() const;

	virtual dpp::message GetMessage(const bool &is_playing_now, const dpp::snowflake &channel_id) const = 0;

	virtual std::string GetTrackData() const = 0;

	virtual int GetOpus(unsigned char* out) = 0;

	virtual bool CanRead() = 0;

protected:
	const unsigned short _duration;

	Track(const unsigned short &duration);

	void SetConverter(AudioToOpus* converter);

	int Convert(char* in, unsigned char* out);

	virtual void OnInit() = 0;

private:
	AudioToOpus* _converter;
	bool _initted;
};

#endif