#include <iostream>
#include "flac_sender.h"
#include "util/logger.h"

FlacSender::FlacSender(const dpp::voiceconn* voiceconn, Track* track) : OpusSender(voiceconn, track), FLAC::Decoder::Stream() {
	this->init();
	this->set_md5_checking(false);  //Disable md5 checking (is it necessary here at all?)
}

void FlacSender::Run() {
	process_until_end_of_stream();
	finish();
}

FLAC__StreamDecoderReadStatus FlacSender::read_callback(FLAC__byte* buffer, size_t* bytes) {
	if (ReadBuffer(buffer, bytes)) return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
	else return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
}

FLAC__StreamDecoderWriteStatus FlacSender::write_callback(const FLAC__Frame* frame, const FLAC__int32* const* buffer) {
	Logger::Debug("WRITE");

	for (int i = 0; i < frame->header.blocksize; i++) {
		stream.put(buffer[0][i]);
		stream.put(buffer[0][i] >> 8);
		stream.put(buffer[1][i]);
		stream.put(buffer[1][i] >> 8);
	}

	stream_size += frame->header.blocksize * 4;

	if (stream_size < OpusSender::PCM_CHUNK_SIZE) return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;

	char pcm_buffer[OpusSender::PCM_CHUNK_SIZE];
	unsigned char opus_buffer[OpusSender::OPUS_CHUNK_SIZE];

	stream.read(pcm_buffer, OpusSender::PCM_CHUNK_SIZE);
	int len = opus_encode(_encoder, (const opus_int16*)pcm_buffer, OpusSender::FRAME_SIZE, opus_buffer, OpusSender::OPUS_CHUNK_SIZE);

	SendOpusData(opus_buffer, len);
	Logger::Warn("Fuck");

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacSender::error_callback(FLAC__StreamDecoderErrorStatus status) {
	if (status == 0) return;

	Logger::Fatal("Error while convert a FLAC data to a raw PCM");
	Logger::Fatal(strerror(status));
}