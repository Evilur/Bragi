#include "play.h"
#include "client/deezer_client.h"

void Play::Exec(const dpp::slashcommand_t &event) {
}

void Play::Exec(const dpp::message_create_t &event) {
	DeezerClient::Search("Korpiklaani Vodka");
}