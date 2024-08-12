#include "wav_to_opus.h"

WavToOpus::WavToOpus() : AudioToOpus() { }

int WavToOpus::Convert(char* in, unsigned char* out) {
	return OpusEncode(in, out);
}