#include "settings.h"
#include "util/logger.h"

#include <fstream>
#include <regex>
#include <iostream>

void Settings::Init() {
	INFO_LOG("Initializing tokens");

    _bot_token = getenv("BRAGI_TOKEN");
    _arl_token = getenv("BRAGI_ARL");

	INFO_LOG("Tokens have been initialized");
}

void Settings::ReinitBotToken() {
	_bot_token = std::string();
	ReinitEmptyTokens();
}

void Settings::ReinitArlToken() {
	_arl_token = std::string();
	ReinitEmptyTokens();
}

char Settings::GetPrefix() { return '!'; }

std::string const &Settings::GetBotToken() { return _bot_token; }

std::string const &Settings::GetArlToken() { return _arl_token; }

void Settings::ReinitEmptyTokens() {
	/* Get tokens */
	if (_bot_token.empty()) {
		std::cout << "\033[0mPlease enter the bot token: ";
		std::cin >> _bot_token;
		INFO_LOG("Bot token was obtained from user input");
	}
	if (_arl_token.empty()) {
		std::cout << "\033[0mPlease enter the arl token: ";
		std::cin >> _arl_token;
		INFO_LOG("Arl token was obtained from user input");
	}
}
