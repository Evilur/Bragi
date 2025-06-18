#ifndef BRAGI_SETTINGS_H
#define BRAGI_SETTINGS_H

class Properties final {
public:
    static constexpr char PREFIX = '!';

	static const char* BotToken();

	static const char* ArlToken();

private:
    inline static const char* _bot_token;
    inline static const char* _arl_token;

    static const char* InitToken(const char* var,
                                 const char* &token_ptr);
};

#endif
