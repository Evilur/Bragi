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
	static constexpr int PCM_CHUNK_SIZE = FRAME_SIZE * CHANNELS * sizeof(short);
	static constexpr int OPUS_CHUNK_SIZE = 1024;

	OpusSender(const dpp::voiceconn* const voiceconn);

	virtual ~OpusSender();

	virtual void Run() = 0;

protected:
	OpusEncoder* _encoder = nullptr;
	const dpp::voiceconn* const _voiceconn;
};

#endif