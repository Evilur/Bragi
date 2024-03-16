#include "settings.h"
#include <fstream>
#include <regex>
#include "util/logger.h"
#include "util/path.h"

std::string Settings::bot_token;
std::string Settings::arl_token;

void Settings::Init() {
	std::ifstream cfg_stream(Path::config_file);  //Open config file for reading
	Logger::Info("Initializing tokens");
	
	if (!cfg_stream.is_open()) {  //If there is not such file
		Logger::Warn("Cannot open config file");
	} else {
		Logger::Info("Reading config file");
		for(std::string line; getline(cfg_stream, line);) {  //Read config file line by line
			std::regex regex(R"([^=\s]+\s*=\s*)");
			if (line.starts_with("bot_token")) bot_token = std::regex_replace(line, regex, "");
			else if (line.starts_with("arl_token")) arl_token = std::regex_replace(line, regex, "");
		}
	}

	cfg_stream.close();  //Close the stream
	if (bot_token.empty() || arl_token.empty()) ReinitTokens();  //Check for empty
	Logger::Info("Tokens have been initialized");
}

char Settings::GetPrefix() { 
	return '!';
}

std::string Settings::GetBotToken() {
	return bot_token;
}

std::string Settings::GetArlToken() {
	return arl_token;
}


void Settings::ReinitTokens() {
	bool needToRewrite = false;
	
	/* Get tokens */
	if (bot_token.empty()) {
		needToRewrite = true;
		std::cout << "Please enter the bot token: ";
		std::cin >> bot_token;
		Logger::Info("Bot token has been gotten from the user input");
	}
	if (arl_token.empty()) {
		needToRewrite = true;
		std::cout << "Please enter the arl token: ";
		std::cin >> arl_token;
		Logger::Info("Arl token has been gotten from the user input");
	}
	
	/* Write config file to the disk */
	if (!needToRewrite) return;
	Logger::Info("Writing new data to the config file");
	std::ofstream cfg_file(Path::config_file);
	cfg_file << "bot_token = " << bot_token << std::endl;
	cfg_file << "arl_token = " << arl_token << std::endl;
}