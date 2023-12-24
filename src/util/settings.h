#ifndef BRAGI_SETTINGS_H
#define BRAGI_SETTINGS_H

#include <fstream>

/* Class for working with settings stored in the filesystem */
class Settings {
public:
	static void Init();
	static std::string GetBotToken();
	static std::string GetArlToken();
private:
	static std::string bot_token;
	static std::string arl_token;

	static void ReinitTokens();
};

#endif
