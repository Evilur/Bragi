#include <iostream>
#include <thread>
#include "flac_sender.h"
#include "util/logger.h"

#include <speex/speex_resampler.h>

FlacSender::FlacSender(const dpp::voiceconn* voiceconn, Track* track) : OpusSender(voiceconn, track), FLAC::Decoder::Stream() {
	this->init();
	this->set_md5_checking(false);  //Disable md5 checking (is it necessary here at all?)
}

FlacSender::~FlacSender() {
	speex_resampler_destroy(dick);
}

void FlacSender::Run() {
	Logger::Debug("Start");
	int err = 0;
	dick = speex_resampler_init(2, 44100, 65268, 10, &err);

	process_until_end_of_stream();
	finish();
	Logger::Debug("End");

	/*while (!stream.eof()) {
		spx_uint32_t in_len = 10240;
		spx_uint32_t out_len = 10240 * 5;
		spx_int16_t in_buffer[in_len];
		spx_int16_t out_buffer[out_len];
		Logger::Warn("Dick");

		stream.read((char*)in_buffer, in_len * 2);
		err = speex_resampler_process_int(dick, 0, in_buffer, &in_len, out_buffer, &out_len);

		fuck.write((char*)out_buffer, out_len * 2);
	}*/

	Logger::Warn("End");
}

FLAC__StreamDecoderReadStatus FlacSender::read_callback(FLAC__byte* buffer, size_t* bytes) {
	if (ReadBuffer(buffer, bytes)) return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
	else return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
}

FLAC__StreamDecoderWriteStatus FlacSender::write_callback(const FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	Logger::Debug("Write");

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

	speex_resampler_process_int(dick, 0, in_left, &in_left_size, out_left, &out_left_size);
	speex_resampler_process_int(dick, 1, in_right, &in_right_size, out_right, &out_right_size);

	for (int i = 0; i < std::max(out_left_size, out_right_size); i++) {
		stream.write((const char*)&(out_left[i]), sizeof(short));
		stream.write((const char*)&(out_right[i]), sizeof(short));
		stream_size += sizeof(short) * 2;
	}

	while (stream_size > PCM_CHUNK_SIZE) {
		char pcm_buffer[PCM_CHUNK_SIZE];
		unsigned char opus_buffer[OPUS_CHUNK_SIZE];

		stream.read(pcm_buffer, PCM_CHUNK_SIZE);
		int len = opus_encode(_encoder, (opus_int16*)pcm_buffer, FRAME_SIZE, opus_buffer, OPUS_CHUNK_SIZE);

		SendOpusData(opus_buffer, len);
		stream_size -= PCM_CHUNK_SIZE;
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacSender::error_callback(FLAC__StreamDecoderErrorStatus status) {
	if (status == 0) return;

	Logger::Fatal("Error while convert a FLAC data to a raw PCM");
	Logger::Fatal(strerror(status));
}