#ifndef BRAGI_SETTINGS_H
#define BRAGI_SETTINGS_H

#include <string>

class Properties final {
public:
    static constexpr char PREFIX = '!';

	static const char* BotToken();

	static const char* ArlToken();

private:
    inline static char* _bot_token;
    inline static char* _arl_token;
};

#endif
