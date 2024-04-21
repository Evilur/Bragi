#include "wav_to_opus.h"

WavToOpus::WavToOpus() : AudioToOpus() { }

int WavToOpus::Convert(char *in, unsigned char *out) {
	return opus_encode(_encoder, (opus_int16*)in, FRAME_SIZE, out, OPUS_CHUNK_SIZE);
}
