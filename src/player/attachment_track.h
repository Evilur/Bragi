#ifndef BRAGI_ATTACHMENT_TRACK_H
#define BRAGI_ATTACHMENT_TRACK_H

#include "track.h"
#include "web/http_client.h"

#include <dpp/dpp.h>

enum AudioType : char {	WAV, FLAC, MP3, OGG };

class AttachmentTrack final : public Track {
public:
	explicit AttachmentTrack(const dpp::snowflake &channel_id, const dpp::attachment* attachment);
	~AttachmentTrack() override;
	
	int GetOpus(unsigned char* out) override;
	bool CanRead() override;
private:
	HttpClient* _http_client;
	AudioType _type;
};

#endif
