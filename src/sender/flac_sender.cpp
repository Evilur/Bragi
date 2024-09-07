#include <iostream>
#include "flac_sender.h"
#include "util/logger.h"

FlacSender::FlacSender(const dpp::voiceconn* voiceconn, Track* track) : OpusSender(voiceconn, track), FLAC::Decoder::Stream() {
	this->init();
	this->set_md5_checking(false);  //Disable md5 checking (is it necessary here at all?)
}

void FlacSender::Run() {
	process_until_end_of_stream();
	finish();
}

FLAC__StreamDecoderReadStatus FlacSender::read_callback(FLAC__byte* buffer, size_t* bytes) {
	if (ReadBuffer(buffer, bytes)) return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
	else return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
}

FLAC__StreamDecoderWriteStatus FlacSender::write_callback(const FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	Logger::Debug("WRITE");

	for (int i = 0; i < frame->header.blocksize; i++) {
		fuck->put(buffer[0][i]);
		fuck->put(buffer[0][i] >> 8);
		fuck->put(buffer[1][i]);
		fuck->put(buffer[1][i] >> 8);
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacSender::error_callback(FLAC__StreamDecoderErrorStatus status) {
	if (status == 0) return;

	Logger::Fatal("Error while convert a FLAC data to a raw PCM");
	Logger::Fatal(strerror(status));
}