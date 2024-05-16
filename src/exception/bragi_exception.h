#ifndef BRAGI_BRAGI_EXCEPTION_H
#define BRAGI_BRAGI_EXCEPTION_H

#include <dpp/dpp.h>

enum ErrorType : char { HARD, SOFT };

class BragiException final : std::exception {
public:
	BragiException(const char* e_data, const dpp::snowflake &channel_id, ErrorType e_type);

	dpp::message Message() const;

private:
	const char* const _e_data;
	const dpp::snowflake _channel_id;
	const ErrorType _e_type;

	dpp::message ForceError() const;

	dpp::message WeakError() const;
};

#endif