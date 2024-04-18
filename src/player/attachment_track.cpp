#include "attachment_track.h"
#include "exception/bragi_exception.h"
#include "util/dictionary.h"
#include "converter/wav_to_opus.h"
#include "converter/opus_converter.h"

std::ofstream fffs("/tmp/F.wav");

AttachmentTrack::AttachmentTrack(const dpp::snowflake &channel_id, const dpp::attachment* attachment) {
	/* Check the filetype */
	if (!attachment->content_type.starts_with("audio")) throw BragiException(DIC_ERROR_IS_NOT_A_FILE, channel_id, HARD);

	_title = attachment->filename;
	_converter = new WavToOpus();
	_type = WAV;
	
	_http = new HttpClient(attachment->url);
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