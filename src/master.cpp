#include <dpp/dpp.h>
#include <util/logger.h>
#include <util/settings.h>

int main() {
	Logger::Init();  //Init the logger
	Settings::Init();  //Init the settings
	
	dpp::cluster bot(Settings::GetBotToken());
	bot.start(dpp::st_wait);
	return 0;
}