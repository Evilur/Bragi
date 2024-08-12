#include <iostream>
#include "flac_to_opus.h"

FlacToOpus::FlacToOpus() : AudioToOpus() {
	FLAC::Decoder::Stream* decoder = this;
	decoder->init();
}

int FlacToOpus::Convert(char* in, unsigned char* out) {
	return OpusEncode(in, out);
}

FLAC__StreamDecoderReadStatus FlacToOpus::read_callback(FLAC__byte* buffer, size_t* bytes) {
	return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
}

::FLAC__StreamDecoderWriteStatus FlacToOpus::write_callback(const ::FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacToOpus::error_callback(::FLAC__StreamDecoderErrorStatus status) {
}