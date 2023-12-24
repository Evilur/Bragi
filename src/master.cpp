#include <dpp/dpp.h>
#include <util/logger.h>

int main() {
	Logger::Init();  //Init the logger
	dpp::cluster bot("ODg4MjkyMzE4NjczNzgwNzY3.GXS_sU.UgKKor15p0427r9YLXAze7vxpmnwKcZtz2zXGU");
	bot.start(dpp::st_wait);
	return 0;
}