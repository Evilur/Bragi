#ifndef BRAGI_JSON_EXCEPTION_H
#define BRAGI_JSON_EXCEPTION_H

#include <exception>
#include <string>

class JsonException final : std::exception {
public:
	explicit JsonException(const char* json_data, const char* query);

	explicit JsonException(const char* json_data, int index);

	friend std::ostream &operator<<(std::ostream &stream, const JsonException &instance);

private:
	const char* _json_data;
	const std::string _wrong_query;
};

#endif