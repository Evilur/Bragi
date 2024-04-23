#ifndef BRAGI_BRAGI_EXCEPTION_H
#define BRAGI_BRAGI_EXCEPTION_H

#include <exception>
#include <dpp/dpp.h>

enum ErrorType : char { HARD, SOFT };

class BragiException final : public std::exception {
public:
	BragiException(const char* e_data, const dpp::snowflake &channel_id, ErrorType e_type);
	
	dpp::message GetMessage();
private:
	const ErrorType _e_type{};
	const char* _e_data;
	const dpp::snowflake _channel_id;

	dpp::message ForceError();
	dpp::message WeakError();
};

#endif
