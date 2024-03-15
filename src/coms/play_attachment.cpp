#include "play_attachment.h"
#include <oggz/oggz.h>

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	/* Get the voice channel the bot is in, in this current guild. */
	dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

	/* If the voice channel was invalid, or there is an issue with it, then tell the user. */
	if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
		event.reply("There was an issue with getting the voice channel. Make sure I'm in a voice channel!");
		return;
	}

	/* load the audio file with oggz */
	OGGZ *track_og = oggz_open("/home/flame/Downloads/CHSV.opus", OGGZ_READ);

	/* If there was an issue reading the file, tell the user and stop */
	if (!track_og) {
		fprintf(stderr, "Error opening file\n");
		event.reply("There was an issue opening the file!");
		return;
	}

	/* set read callback, this callback will be called on packets with the serialno,
	 * -1 means every packet will be handled with this callback.
	 */
	oggz_set_read_callback(
			track_og, -1,
			[](OGGZ *oggz, oggz_packet *packet, long serialno,
			   void *user_data) {
				dpp::voiceconn *voiceconn = (dpp::voiceconn *)user_data;

				/* send the audio */
				voiceconn->voiceclient->send_audio_opus(packet->op.packet,
				                                        packet->op.bytes);

				/* make sure to always return 0 here, read more on oggz documentation */
				return 0;
			},
			/* this will be the value of void *user_data */
			(void *)v
	);

	// read loop
	while (v && v->voiceclient && !v->voiceclient->terminating) {
		/* you can tweak this to whatever. Here I use BUFSIZ, defined in
		 * stdio.h as 8192.
		 */
		static const constexpr long CHUNK_READ = BUFSIZ * 2;

		const long read_bytes = oggz_read(track_og, CHUNK_READ);

		/* break on eof */
		if (!read_bytes) {
			break;
		}
	}

	/* Don't forget to free the memory */
	oggz_close(track_og);

	event.reply("Finished playing the audio file!");
}

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	
}

dpp::message PlayAttachment::Message(dpp::cluster &bot) {
	return dpp::message("TEST");
}
