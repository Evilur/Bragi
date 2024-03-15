#include "play_attachment.h"

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	event.reply("Хуй");
}

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	
}

dpp::message PlayAttachment::Message(dpp::cluster &bot) {
	return dpp::message();
}
