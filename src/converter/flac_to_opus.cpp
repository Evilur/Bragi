#include <iostream>
#include "flac_to_opus.h"

FlacToOpus::FlacToOpus() : AudioToOpus(), FLAC::Decoder::Stream() {
	_http = new HttpClient("localhost/data.flac");
	_fuck.open("/tmp/fuck.flac");

	this->init();
	this->set_md5_checking(false);  //Disable md5 checking (is it necessary here at all?)
	this->process_until_end_of_stream();
}

int FlacToOpus::Convert(char* in, unsigned char* out) {
	return OpusEncode(in, out);
}

FLAC__StreamDecoderReadStatus FlacToOpus::read_callback(FLAC__byte* buffer, size_t* bytes) {
	if (!_http->CanRead()) {
		std::cout << "EOF\n";
		exit(200);
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	}

	std::cout << "READ\n";

	/* Read to the temporary buffer */
	FLAC__byte tmp_buffer[*bytes];
	_http->Read((char*)tmp_buffer, *bytes);

	/* Clone the temporary buffer data to the master buffer */
	std::copy(tmp_buffer, tmp_buffer + *bytes, buffer);

	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus FlacToOpus::write_callback(const FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	std::cout << "WRITE\n";
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacToOpus::error_callback(FLAC__StreamDecoderErrorStatus status) {
	std::cout << strerror(status) << std::endl;
}