#ifndef BRAGI_SETTINGS_H
#define BRAGI_SETTINGS_H

class Properties final {
public:
    static constexpr char PREFIX = '!';

	static const char* BotToken();

	static const char* ArlToken();

private:
    inline static const char* _bot_token = nullptr;
    inline static const char* _arl_token = nullptr;

    /**
     * Initialize the token
     * @param var The name of the token
     * @param token_ptr The buffer to store the result
     */
    static void InitToken(const char* var,
                          const char* &token_ptr);
};

#endif
