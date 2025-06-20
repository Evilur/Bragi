#include "list.h"
#include "base/bragi.h"
#include "exception/bragi_exception.h"

void List::Exec(const dpp::slashcommand_t &event) {
	/* Send the message to the channel */
	try { event.reply(Exec(event.command.guild_id, event.command.channel_id)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

void List::Exec(const dpp::message_create_t &event) {
	/* Send the message to the channel */
	try { event.send(Exec(event.msg.guild_id, event.msg.channel_id)); }
	catch (BragiException &exception) { event.send(exception.Message()); }
}

dpp::message List::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id) {
	return Bragi::Get(guild_id)->ListCommand(channel_id);
}
