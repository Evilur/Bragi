#ifndef BRAGI_OPUS_SENDER_H
#define BRAGI_OPUS_SENDER_H

#include "opus/opus.h"
#include "player/track/track.h"

#include <dpp/discordclient.h>

class OpusSender {
public:
	static constexpr int FREQ = 48000;
	static constexpr int FRAME_SIZE = 2880;
	static constexpr int CHANNELS = 2;
	static constexpr int PCM_CHUNK_SIZE = FRAME_SIZE * CHANNELS * sizeof(opus_int16);
	static constexpr int OPUS_CHUNK_SIZE = 1024;

	OpusSender(const dpp::voiceconn* voiceconn, Track* track);

	virtual ~OpusSender();

	virtual void Run() = 0;

protected:
	bool ReadBuffer(unsigned char* buffer, unsigned long* buffer_size);

private:
	OpusEncoder* _encoder;
	const dpp::voiceconn* _voiceconn;
	Track* _track;
};

#endif