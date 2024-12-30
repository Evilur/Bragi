template<typename F>
FlacSender<F>::FlacSender(const dpp::voiceconn* const voiceconn, const byte speed_percent, F* read_buffer)  :
		OpusSender(voiceconn, speed_percent), FLAC::Decoder::Stream(), _read_buffer_func(read_buffer) {
	/* Init the flac decoder */
	this->init();
}

template<typename F>
void FlacSender<F>::Run() {
	/* Decode all FLAC data and send OPUS to the discord */
	process_until_end_of_stream();
	finish();
}

template<typename F>
FLAC__StreamDecoderReadStatus FlacSender<F>::read_callback(byte* buffer, unsigned long* bytes) {
	if ((*_read_buffer_func)(buffer, bytes)) return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
	else return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
}

template<typename F>
FLAC__StreamDecoderWriteStatus FlacSender<F>::write_callback(const FLAC__Frame* frame, const int* const* buffer) {
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

template<typename F>
void FlacSender<F>::error_callback(FLAC__StreamDecoderErrorStatus status) {
	if (status == 0) return;

	Logger::Warn("Error while convert a FLAC data to a raw PCM");
	Logger::Warn(strerror(status));
}