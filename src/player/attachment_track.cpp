#include "attachment_track.h"
#include "util/dictionary.h"
#include "converter/wav_to_opus.h"
#include "exception/bragi_exception.h"

AttachmentTrack::AttachmentTrack(const dpp::snowflake &channel_id, const dpp::attachment* attachment) {
	/* Check the filetype */
	if (!attachment->content_type.starts_with("audio")) throw BragiException(DIC_ERROR_IS_NOT_A_FILE, channel_id, HARD);

	/* Fill the fields */
	_http_client = new HttpsClient(attachment->url);
	_title = attachment->filename;
	_type = WAV;

	/* Init the converter */
	_converter = new WavToOpus();
}

AttachmentTrack::~AttachmentTrack() {
	delete[] _http_client;
	_http_client = nullptr;
}

int AttachmentTrack::GetOpus(unsigned char* out) {
	char pcm_chunk[AudioToOpus::PCM_CHUNK_SIZE];
	_http_client->Read(pcm_chunk, AudioToOpus::PCM_CHUNK_SIZE);
	return _converter->Convert(pcm_chunk, out);
}

bool AttachmentTrack::CanRead() { return _http_client->CanRead(); }