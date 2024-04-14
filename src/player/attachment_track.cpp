#include "attachment_track.h"
#include "exception/bragi_exception.h"
#include "util/logger.h"
#include "util/dictionary.h"
#include "converter/wav_to_opus.h"

AttachmentTrack::AttachmentTrack(const dpp::snowflake &channel_id, const dpp::attachment* attachment) {
	/* Check the filetype */
	if (!attachment->content_type.starts_with("audio")) throw BragiException(DIC_ERROR_IS_NOT_A_FILE, channel_id, HARD);
	
	title = attachment->filename;
	converter = new WavToOpus();
	_type = WAV;
	
	Logger::Debug(attachment->url);
	_stream = new boost::asio::ip::tcp::iostream(attachment->url, "http");
}

AttachmentTrack::~AttachmentTrack() {
	_stream->close();
}

int AttachmentTrack::GetOpus(unsigned char *out) {
	return 0;
}