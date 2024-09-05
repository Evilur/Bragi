#ifndef BRAGI_OPUS_SENDER_H
#define BRAGI_OPUS_SENDER_H

#include <dpp/discordclient.h>
#include "opus/opus.h"

class OpusSender {
public:
	static constexpr int FREQ = 48000;
	static constexpr int FRAME_SIZE = 2880;
	static constexpr int CHANNELS = 2;
	static constexpr int PCM_CHUNK_SIZE = FRAME_SIZE * CHANNELS * sizeof(opus_int16);
	static constexpr int OPUS_CHUNK_SIZE = 1024;

	OpusSender(const dpp::voiceconn* voiceconn);

	virtual ~OpusSender();

	virtual void Send(const char* in, const int size) = 0;

private:
	OpusEncoder* _encoder;
	const dpp::voiceconn* _voiceconn;
};

#endif