#include "flac_to_opus.h"

FlacToOpus::FlacToOpus() : AudioToOpus() { }

int FlacToOpus::Convert(char* in, unsigned char* out) {
	return opus_encode(_encoder, (opus_int16*)in, FRAME_SIZE, out, OPUS_CHUNK_SIZE);
}