#ifndef BRAGI_ERROR_H
#define BRAGI_ERROR_H

#include <cluster.h>

class Error {
public:
	static dpp::message Force(const char *e_data, dpp::snowflake channel_id);
	static dpp::message Soft(const char *e_data, dpp::snowflake channel_id);
};

#endif
