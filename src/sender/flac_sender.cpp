#include <iostream>
#include "flac_sender.h"
#include "util/logger.h"
#include "master.h"

#include <speex/speex_resampler.h>

FlacSender::FlacSender(const dpp::voiceconn* voiceconn, Track* track) : OpusSender(voiceconn, track), FLAC::Decoder::Stream() {
	/* Init the flac decoder */
	this->init();

	/* Init the resampler */
	_resampler = speex_resampler_init(2, 44100, 48000, 10, nullptr);
}

FlacSender::~FlacSender() {
	/* Destroy the resampler */
	speex_resampler_destroy(_resampler);
	_resampler = nullptr;
}

void FlacSender::Run() {
	/* Decode all FLAC data and send OPUS to the discord */
	process_until_end_of_stream();
	finish();
}

FLAC__StreamDecoderReadStatus FlacSender::read_callback(FLAC__byte* buffer, size_t* bytes) {
	if (ReadBuffer(buffer, bytes)) return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
	else return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
}

FLAC__StreamDecoderWriteStatus FlacSender::write_callback(const FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	unsigned int in_left_size = frame->header.blocksize;
	unsigned int in_right_size = frame->header.blocksize;
	short in_left[frame->header.blocksize];
	short in_right[frame->header.blocksize];

	unsigned int out_left_size = frame->header.blocksize * 5;
	unsigned int out_right_size = frame->header.blocksize * 5;
	short out_left[out_left_size];
	short out_right[out_right_size];

	for (int i = 0; i < frame->header.blocksize; i++) {
		in_left[i] = buffer[0][i];
		in_right[i] = buffer[1][i];
	}

	speex_resampler_process_int(_resampler, 0, in_left, &in_left_size, out_left, &out_left_size);
	speex_resampler_process_int(_resampler, 1, in_right, &in_right_size, out_right, &out_right_size);

	for (int i = 0; i < std::max(out_left_size, out_right_size); i++) {
		_stream.write((const char*)&(out_left[i]), sizeof(short));
		_stream.write((const char*)&(out_right[i]), sizeof(short));
		_stream_size += sizeof(short) * 2;
	}

	while (_stream_size > PCM_CHUNK_SIZE) {
		char pcm_buffer[PCM_CHUNK_SIZE];
		unsigned char opus_buffer[OPUS_CHUNK_SIZE];

		_stream.read(pcm_buffer, PCM_CHUNK_SIZE);
		int len = opus_encode(_encoder, (short*)pcm_buffer, FRAME_SIZE, opus_buffer, OPUS_CHUNK_SIZE);

		_voiceconn->voiceclient->send_audio_opus(opus_buffer, len, 60);
		_stream_size -= PCM_CHUNK_SIZE;
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacSender::error_callback(FLAC__StreamDecoderErrorStatus status) {
	if (status == 0) return;

	Logger::Fatal("Error while convert a FLAC data to a raw PCM");
	Logger::Fatal(strerror(status));
}