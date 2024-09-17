#include "settings.h"
#include "util/logger.h"
#include "util/path.h"

#include <fstream>
#include <regex>

std::string Settings::_bot_token;
std::string Settings::_arl_token;

void Settings::Init() {
	std::ifstream cfg_stream(Path::CONFIG_FILE);  //Open config file for reading
	Logger::Info("Initializing tokens");

	if (!cfg_stream.is_open()) {  //If there is not such file
		Logger::Warn("Cannot open config file");
	} else {
		Logger::Info("Reading config file");
		for (std::string line; getline(cfg_stream, line);) {  //Read config file line by line
			std::regex regex(R"([^=\s]+\s*=\s*)");
			if (line.starts_with("bot_token")) _bot_token = std::regex_replace(line, regex, "");
			else if (line.starts_with("arl_token")) _arl_token = std::regex_replace(line, regex, "");
		}
	}

	cfg_stream.close();  //Close the stream
	if (_bot_token.empty() || _arl_token.empty()) ReinitTokens();  //Check for empty
	Logger::Info("Tokens have been initialized");
}

char Settings::GetPrefix() { return '!'; }

std::string const &Settings::GetBotToken() { return _bot_token; }

std::string const &Settings::GetArlToken() { return _arl_token; }


void Settings::ReinitTokens() {
	bool need_to_rewrite = false;

	/* Get tokens */
	if (_bot_token.empty()) {
		need_to_rewrite = true;
		std::cout << "\033[0mPlease enter the bot token: ";
		std::cin >> _bot_token;
		Logger::Info("Bot token has been gotten from the user input");
	}
	if (_arl_token.empty()) {
		need_to_rewrite = true;
		std::cout << "\033[0mPlease enter the arl token: ";
		std::cin >> _arl_token;
		Logger::Info("Arl token has been gotten from the user input");
	}

	/* Write config file to the disk */
	if (!need_to_rewrite) return;
	Logger::Info("Writing new data to the config file");
	std::ofstream cfg_file(Path::CONFIG_FILE);
	cfg_file << "bot_token = " << _bot_token << std::endl;
	cfg_file << "arl_token = " << _arl_token << std::endl;
}