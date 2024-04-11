#include "attachment_track.h"
#include "util/bragi_exception.h"
#include "util/logger.h"
#include "util/dictionary.h"
#include "converter/wav_to_opus.h"

AttachmentTrack::AttachmentTrack(const dpp::snowflake &channel_id, const dpp::attachment* attachment) {
	/* Check the filetype */
	if (!attachment->content_type.starts_with("audio")) throw BragiException(DIC_ERROR_IS_NOT_A_FILE, channel_id, Hard);
	
	title = attachment->filename;
	type = wav;
	converter = new WavToOpus();
	stream = boost::asio::ip::tcp::iostream(attachment->url, "80");
}

AttachmentTrack::~AttachmentTrack() {
	stream.close();
}

int AttachmentTrack::GetOpus(unsigned char *out) {
	if (stream.peek() == EOF) Logger::Debug(123);
	char* raw_chunk = new char[OpusConverter::PCM_CHUNK_SIZE];
	stream.read(raw_chunk, OpusConverter::PCM_CHUNK_SIZE);
	int len = converter->Convert(raw_chunk, out);
	if (len == 0) is_end = true;
	delete[] raw_chunk;
	return len;
}
