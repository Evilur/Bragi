#include <iostream>
#include "flac_sender.h"
#include "util/logger.h"

FlacSender::FlacSender(const dpp::voiceconn* voiceconn) : OpusSender(voiceconn), FLAC::Decoder::Stream() {
	this->init();
	this->set_md5_checking(false);  //Disable md5 checking (is it necessary here at all?)
}

void FlacSender::Send(const char* in, const int size) {
	std::cout << "DICK DICK \n";
}

FLAC__StreamDecoderReadStatus FlacSender::read_callback(FLAC__byte* buffer, size_t* bytes) {
	/* Read to the temporary buffer */
	FLAC__byte tmp_buffer[*bytes];

	/* Clone the temporary buffer data to the master buffer */
	std::copy(tmp_buffer, tmp_buffer + *bytes, buffer);

	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus FlacSender::write_callback(const FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	for (int i = 0; i < frame->header.blocksize; i++) {
		_stream.put(buffer[0][i]);
		_stream.put(buffer[0][i] >> 8);
		_stream.put(buffer[1][i]);
		_stream.put(buffer[1][i] >> 8);
	}

	_stream_size += frame->header.blocksize * 4;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacSender::error_callback(FLAC__StreamDecoderErrorStatus status) {
	Logger::Fatal("Error while convert a FLAC data to a raw PCM");
	Logger::Fatal(strerror(status));
}