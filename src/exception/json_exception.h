#ifndef BRAGI_JSON_EXCEPTION_H
#define BRAGI_JSON_EXCEPTION_H

#include <exception>
#include <string>

class JsonException final {
public:
	explicit JsonException(const char* json_data, const char* query);

	explicit JsonException(const char* json_data, int index);
};

#endif