#include "attachment_track.h"
#include "exception/bragi_exception.h"
#include "converter/audio_to_opus.h"
#include "converter/wav_to_opus.h"
#include "util/dictionary.h"

AttachmentTrack::AttachmentTrack(const dpp::snowflake &channel_id, const dpp::attachment* attachment) {
	/* Check the filetype */
	//if (!attachment->content_type.starts_with("audio")) throw BragiException(DIC_ERROR_IS_NOT_A_FILE, channel_id, HARD);

	_http_client = new HttpClient(/*attachment->url*/"http://localhost/TEST.pcm");
	_title = "attachment->filename";
	_type = WAV;

	_converter = new WavToOpus();
}

AttachmentTrack::~AttachmentTrack() {
	delete[] _http_client;
	_http_client = nullptr;
}

int AttachmentTrack::GetOpus(unsigned char* out) {
	char pcm_chunk[AudioToOpus::PCM_CHUNK_SIZE];
	_http_client->Read(pcm_chunk, AudioToOpus::PCM_CHUNK_SIZE);
	int len = _converter->Convert(pcm_chunk, out);
	return len;
}

bool AttachmentTrack::CanRead() { return _http_client->CanRead(); }
