template<typename F>
FlacSender<F>::FlacSender(dpp::discord_voice_client* const voiceclient, const byte speed_percent, const F* const read_buffer)  :
		OpusSender(voiceclient, speed_percent), FLAC::Decoder::Stream(), _read_buffer(read_buffer) {
	/* Init the flac decoder */
	this->init();
}

template<typename F>
void FlacSender<F>::Run() {
	/* Decode all FLAC data and send OPUS to the discord */
	process_until_end_of_stream();
	finish();
	InsertEOF();
}

template<typename F>
FLAC__StreamDecoderReadStatus FlacSender<F>::read_callback(byte* buffer, unsigned long* buffer_size) {
	if (!IsAborted() && (*_read_buffer)(buffer, buffer_size)) return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
	else {
		*buffer_size = 0;
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	};
}

template<typename F>
FLAC__StreamDecoderWriteStatus FlacSender<F>::write_callback(const FLAC__Frame* frame, const int* const* buffer) {
	/* Read the input data */
	short in_left[frame->header.blocksize], in_right[frame->header.blocksize];
	for (int i = 0; i < frame->header.blocksize; i++) {
		in_left[i] = buffer[0][i];
		in_right[i] = buffer[1][i];
	}

	/* Send the data to the discord */
	SendData(in_left, in_right, frame->header.blocksize);

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

template<typename F>
void FlacSender<F>::error_callback(FLAC__StreamDecoderErrorStatus status) {
	if (status == 0) return;

	ERROR_LOG("Error while convert a FLAC data to a raw PCM");
	ERROR_LOG(strerror(status));
	Abort();
	sleep(1);  //This is for debugging
}