#ifndef BRAGI_ATTACHMENT_TRACK_H
#define BRAGI_ATTACHMENT_TRACK_H

#include "track.h"

#include <dpp/dpp.h>
#include <boost/asio/ip/tcp.hpp>

enum AudioType : char {	wav, flac, mp3, ogg };

class AttachmentTrack : public Track {
public:
	explicit AttachmentTrack(const dpp::snowflake &channel_id, const dpp::attachment* attachment);
	~AttachmentTrack() override;
	
	int GetOpus(unsigned char *out) override;
private:
	boost::asio::ip::tcp::iostream stream;
	AudioType type;
};

#endif
