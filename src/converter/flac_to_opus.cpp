#include "flac_to_opus.h"

FlacToOpus::FlacToOpus() : AudioToOpus() { }

int FlacToOpus::Convert(char* in, unsigned char* out) {
	return opus_encode(_encoder, (opus_int16*)in, FRAME_SIZE, out, OPUS_CHUNK_SIZE);
}

FLAC__StreamDecoderReadStatus FlacToOpus::read_callback(FLAC__byte* buffer, size_t* bytes) {
	return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
}

::FLAC__StreamDecoderWriteStatus FlacToOpus::write_callback(const ::FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacToOpus::error_callback(::FLAC__StreamDecoderErrorStatus status) {
}