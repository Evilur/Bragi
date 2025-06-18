#include "properties.h"
#include "util/logger.h"

#include <cstdlib>

const char* Properties::BotToken() {
    /* If we already have a token, return it */
    if (_bot_token) return _bot_token;

    /* If we have NOT a token yet, obtain the token from the env */
    _bot_token = getenv("BOT_TOKEN");

    /* Print the result */
    if (_bot_token) {
        INFO_LOG("Obtaining a bot token from the environment");
    } else {
        FATAL_LOG("Can't obtain a bot token from the environment");
        exit(1);
    }

    /* Return the token */
    return _bot_token;
}

const char* Properties::ArlToken() {
    /* If we already have a token, return it */
    if (_arl_token) return _arl_token;

    /* If we have NOT a token yet, obtain the token from the env */
    _arl_token = getenv("ARL_TOKEN");

    /* Print the result */
    if (_arl_token) {
        INFO_LOG("Obtaining an arl token from the environment");
    } else {
        FATAL_LOG("Can't obtain an arl token from the environment");
        exit(2);
    }

    /* Return the token */
    return _arl_token;
}
