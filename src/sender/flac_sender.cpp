#include <iostream>
#include "flac_to_opus.h"

FlacToOpus::FlacToOpus() : OpusSender(), FLAC::Decoder::Stream() {
	this->init();
	this->set_md5_checking(false);  //Disable md5 checking (is it necessary here at all?)
}

int FlacToOpus::Convert(char* in, unsigned char* out) {
	
	return Send(in, out);
}

FLAC__StreamDecoderReadStatus FlacToOpus::read_callback(FLAC__byte* buffer, size_t* bytes) {
	/* Read to the temporary buffer */
	FLAC__byte tmp_buffer[*bytes];

	/* Clone the temporary buffer data to the master buffer */
	std::copy(tmp_buffer, tmp_buffer + *bytes, buffer);

	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus FlacToOpus::write_callback(const FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	for (int i = 0; i < frame->header.blocksize; i++) {
		_stream.put(buffer[0][i]);
		_stream.put(buffer[0][i] >> 8);
		_stream.put(buffer[1][i]);
		_stream.put(buffer[1][i] >> 8);
	}

	_stream_size += frame->header.blocksize * 4;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacToOpus::error_callback(FLAC__StreamDecoderErrorStatus status) {
	std::cout << strerror(status) << std::endl;
}