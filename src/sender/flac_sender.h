#ifndef BRAGI_FLAC_SENDER_H
#define BRAGI_FLAC_SENDER_H

#include "opus_sender.h"
#include "http/http_client.h"

#include <FLAC++/decoder.h>
#include <fstream>

class FlacSender final : public OpusSender, private FLAC::Decoder::Stream {
public:
	FlacSender(const dpp::voiceconn* voiceconn, Track* track);

	void Run() override;

	FLAC__StreamDecoderReadStatus read_callback(FLAC__byte* buffer, size_t* bytes) override;

	FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame* frame, const FLAC__int32* const* buffer) override;

	void error_callback(FLAC__StreamDecoderErrorStatus status) override;

private:
	const char* _flac_buffer;
	size_t _flac_buffer_size;

	std::ofstream* fuck = new std::ofstream("/tmp/test.pcm");
};

#endif