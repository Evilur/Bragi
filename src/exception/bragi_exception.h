#ifndef BRAGI_BRAGI_EXCEPTION_H
#define BRAGI_BRAGI_EXCEPTION_H

#include <dpp/dpp.h>

class BragiException final {
public:
	enum Type : char { HARD, SOFT };

	BragiException(const char* e_data, const dpp::snowflake &channel_id, Type e_type);

	dpp::message Message() const;

private:
	const char* const _e_data;
	const dpp::snowflake _channel_id;
	const Type _e_type;

	dpp::message ForceError() const;

	dpp::message WeakError() const;
};

#endif