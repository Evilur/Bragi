#ifndef BRAGI_BRAGI_EXCEPTION_H
#define BRAGI_BRAGI_EXCEPTION_H

#include <exception>
#include "dpp/dpp.h"

enum ErrorType : char { HARD, SOFT };

class BragiException : public std::exception {
public:
	BragiException(const char* e_data, const dpp::snowflake &channel_id, ErrorType e_type = ErrorType::HARD);
	
	dpp::message GetMessage();
private:
	const ErrorType e_type{};
	const char* e_data;
	const dpp::snowflake channel_id;

	dpp::message ForceError();
	dpp::message WeakError();
};

#endif
