#include <iostream>
#include "flac_to_opus.h"

FlacToOpus::FlacToOpus() : AudioToOpus(), FLAC::Decoder::Stream() {
	_http = new HttpClient("localhost/data.flac");
	_fuck.open("/tmp/fuck.flac");

	this->init();
	this->process_until_end_of_stream();
}

int FlacToOpus::Convert(char* in, unsigned char* out) {
	return OpusEncode(in, out);
}

FLAC__StreamDecoderReadStatus FlacToOpus::read_callback(FLAC__byte* buffer, size_t* bytes) {
	_http->Read((char*)_buffer, (long)_buffer_size);

	if (_http->CanRead()) {
		std::cout << "OK\n";
		return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
	} else {
		//exit(200);
		std::cout << "THE END\n";
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	}
}

FLAC__StreamDecoderWriteStatus FlacToOpus::write_callback(const FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	std::cout << "WRITE\n";

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacToOpus::error_callback(FLAC__StreamDecoderErrorStatus status) {
	std::cout << strerror(status) << std::endl;
}