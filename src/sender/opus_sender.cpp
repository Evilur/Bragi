#include "opus_sender.h"

OpusSender::OpusSender(dpp::discord_voice_client* const voiceclient, const u_byte speed_percent) :
		_voiceclient(voiceclient), _resampler_output_freq(FREQ * 100 / speed_percent) {
	_encoder = opus_encoder_create(FREQ, CHANNELS, OPUS_APPLICATION_AUDIO, nullptr);
	_resampler = speex_resampler_init(2, RESAMPLER_INPUT_FREQ, _resampler_output_freq, 10, nullptr);
}

OpusSender::~OpusSender() {
	/* Destroy the encoder */
	opus_encoder_destroy(_encoder);
	_encoder = nullptr;

	/* Destroy the resampler */
	speex_resampler_destroy(_resampler);
	_resampler = nullptr;
}

void OpusSender::Abort() { _is_aborted = true; }

bool OpusSender::IsAborted() const { return _is_aborted; }

void OpusSender::SendData(const short* in_left, const short* in_right, const unsigned short in_size) {
	/* Get the output pcm array size with some extra space for overflow protection */
	const unsigned int out_size = in_size * _resampler_output_freq / RESAMPLER_INPUT_FREQ + 64;

	/* Init arrays for output (resampled) data */
	unsigned int out_left_size = out_size, out_right_size = out_size;
	short out_left[out_left_size], out_right[out_right_size];

	/* Resample input data and put the result to the output arrays */
	{
		unsigned int in_left_size = in_size, in_right_size = in_size;
		speex_resampler_process_int(_resampler, 0, in_left, &in_left_size, out_left, &out_left_size);
		speex_resampler_process_int(_resampler, 1, in_right, &in_right_size, out_right, &out_right_size);
	}

	/* Read the data and send it to the discord */
	for (int i = 0; i < out_left_size; i++) {
		/* Read the data */
		*_pcm_buffer_ptr++ = out_left[i];
		*_pcm_buffer_ptr++ = out_right[i];

		/* If we have enough, send the data to the discod */
		if (_pcm_buffer_ptr < _pcm_buffer_end) continue;

		/* Convert ot OPUS and send the data to the discord */
		unsigned char opus_buffer[OPUS_CHUNK_SIZE];
		int len = opus_encode(_encoder, _pcm_buffer, FRAME_SIZE, opus_buffer, OPUS_CHUNK_SIZE);
		_voiceclient->send_audio_opus(opus_buffer, len, 60);

		/* Reset the pointer */
		_pcm_buffer_ptr = _pcm_buffer;
	}
}

void OpusSender::InsertEOF() {
	/* Insert the EOF marker, if not aborted */
	if (!_is_aborted) _voiceclient->insert_marker();
}