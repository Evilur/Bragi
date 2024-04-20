#include "attachment_track.h"
#include "exception/bragi_exception.h"
#include "converter/opus_converter.h"
#include "converter/wav_to_opus.h"

AttachmentTrack::AttachmentTrack(const dpp::snowflake &channel_id, const dpp::attachment* attachment) {
	/* Check the filetype */
	/*if (!attachment->content_type.starts_with("audio")) throw BragiException(DIC_ERROR_IS_NOT_A_FILE, channel_id, HARD);

	_title = attachment->filename;
	_type = WAV;*/

	_converter = new WavToOpus();
	_http = new HttpClient("http://localhost/CHSV.wav");
}

AttachmentTrack::~AttachmentTrack() {
	delete _http;
}

int AttachmentTrack::GetOpus(unsigned char *out) {
	char* pcm_chunk = new char[OpusConverter::PCM_CHUNK_SIZE];
	_http->Read(pcm_chunk, OpusConverter::PCM_CHUNK_SIZE);
	int len = _converter->Convert(pcm_chunk, out);
	delete[] pcm_chunk;
	return len;
}