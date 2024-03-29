#include "play_attachment.h"
#include "guild//guild.h"
#include "util/logger.h"
#include "coms/join.h"
#include <oggz/oggz.h>
#include <unistd.h>

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	Guild* guild = Guild::Get(event.command.guild_id);
}

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	
}

dpp::message PlayAttachment::Message(dpp::cluster &bot) {
	return dpp::message("TEST");
}
