#ifndef BRAGI_FLAC_SENDER_H
#define BRAGI_FLAC_SENDER_H

#include "opus_sender.h"
#include "web/http_client.h"
#include "util/logger.h"

#include <FLAC++/decoder.h>
#include <fstream>
#include <speex/speex_resampler.h>

template<typename F>
class FlacSender final : public OpusSender, private FLAC::Decoder::Stream {
public:
	FlacSender(dpp::discord_voice_client* const voiceclient, const u_byte speed_percent, const F* const read_buffer);

	void Run() override;

private:
	const F* const _read_buffer;

	FLAC__StreamDecoderReadStatus read_callback(u_byte* buffer, unsigned long* buffer_size) override;

	FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame* frame, const int* const* buffer) override;

	void error_callback(FLAC__StreamDecoderErrorStatus status) override;
};

#include "flac_sender.tpp"

#endif