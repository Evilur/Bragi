#ifndef BRAGI_FLAC_SENDER_H
#define BRAGI_FLAC_SENDER_H

#include "opus_sender.h"
#include "http/http_client.h"
#include "util/logger.h"

#include <FLAC++/decoder.h>
#include <fstream>
#include <speex/speex_resampler.h>

template<typename F>
class FlacSender final : private OpusSender, private FLAC::Decoder::Stream {
public:
	FlacSender(const dpp::voiceconn* const voiceconn, const byte speed_percent, F* read_buffer);

	void Run() override;

private:
	const F* _read_buffer_func;

	FLAC__StreamDecoderReadStatus read_callback(byte* buffer, unsigned long* buffer_size) override;

	FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame* frame, const int* const* buffer) override;

	void error_callback(FLAC__StreamDecoderErrorStatus status) override;
};

#include "flac_sender.tpp"

#endif