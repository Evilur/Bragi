#include "properties.h"
#include "util/logger.h"

#include <cstdlib>

const char* Properties::BotToken() {
    /* If we already have a token */
    if (_bot_token) return _bot_token;

    /* If we have NOT a token yet */
    InitToken("BOT_TOKEN", _bot_token);
    return _bot_token;
}

const char* Properties::ArlToken() {
    /* If we already have a token */
    if (_arl_token) return _arl_token;

    /* If we have NOT a token yet */
    InitToken("ARL_TOKEN", _arl_token);
    return _arl_token;
}

void Properties::InitToken(const char* var,
                           const char* &token_ptr) {
    /* Obtain the token from the env */
    token_ptr = getenv(var);

    /* Check the result and print the _message according to it */
    if (token_ptr) {
        INFO_LOG("%s has been obtained from the environment", var);
    } else {
        FATAL_LOG("Can't obtain %s from the environment", var);
        exit(1);
    }
}
