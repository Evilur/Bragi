#ifndef BRAGI_SETTINGS_H
#define BRAGI_SETTINGS_H

#include <fstream>

/* Class for working with settings stored in the filesystem */
class Settings final {
public:
	static void Init();

	static void ReinitBotToken();

	static void ReinitArlToken();

	static char GetPrefix();

	static std::string const &GetBotToken();

	static std::string const &GetArlToken();

private:
	static inline std::string _bot_token;
	static inline std::string _arl_token;

	static void ReinitEmptyTokens();
};

#endif