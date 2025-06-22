#ifndef BRAGI_BRAGI_EXCEPTION_H
#define BRAGI_BRAGI_EXCEPTION_H

#include <dpp/dpp.h>

class BragiException final : std::exception {
public:
	enum Type { HARD, SOFT };

	BragiException(const char* e_data, Type e_type);

	dpp::message GetMessage() const;

private:
	const char* const _message;
	const Type _type;
};

#endif