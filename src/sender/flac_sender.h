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
	FlacSender(const dpp::voiceconn* const voiceconn, F* read_buffer);

	~FlacSender();

	void Run() override;

	FLAC__StreamDecoderReadStatus read_callback(FLAC__byte* buffer, size_t* bytes) override;

	FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame* frame, const FLAC__int32* const* buffer) override;

	void error_callback(FLAC__StreamDecoderErrorStatus status) override;

private:
	SpeexResamplerState* _resampler = nullptr;
	std::stringstream _stream;
	signed long _stream_size = 0;
	const F* _read_buffer;
};

#include "flac_sender.tpp"

#endif