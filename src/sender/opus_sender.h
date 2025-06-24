#ifndef BRAGI_OPUS_SENDER_H
#define BRAGI_OPUS_SENDER_H

#include "opus/opus.h"
#include "master.h"

#include <dpp/discordclient.h>
#include <speex/speex_resampler.h>

class OpusSender {
public:
	OpusSender(dpp::discord_voice_client* const voiceclient, const unsigned char speed_percent);

	virtual ~OpusSender();

	virtual void Run() = 0;

	void Abort();

	bool IsAborted() const;

protected:
	void SendData(const short* in_left, const short* in_right, const unsigned short in_size);

	void InsertEOF();

private:
	static constexpr int FREQ = 48000;
	static constexpr int FRAME_SIZE = 2880;
	static constexpr int CHANNELS = 2;
	static constexpr int PCM_CHUNK_SIZE = FRAME_SIZE * CHANNELS;
	static constexpr int OPUS_CHUNK_SIZE = 1024;
	static constexpr int RESAMPLER_INPUT_FREQ = 44100;
	const int _resampler_output_freq;

	bool _is_aborted = false;
	short _pcm_buffer[PCM_CHUNK_SIZE];
	short* _pcm_buffer_ptr = _pcm_buffer;

	const short* const _pcm_buffer_end = _pcm_buffer + PCM_CHUNK_SIZE;
	dpp::discord_voice_client* const _voiceclient;
	SpeexResamplerState* _resampler;
	OpusEncoder* _encoder;
};

#endif