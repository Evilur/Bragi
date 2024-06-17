#include "json_exception.h"
#include "util/logger.h"

#include <format>

JsonException::JsonException(const char* const json_data, const char* query) : _json_data(json_data), _wrong_query(
		std::format("Can't find the object by key \"{}\" in the json data:", query)) { }

JsonException::JsonException(const char* const json_data, const int index) : _json_data(json_data), _wrong_query(
		std::format("Can't find the object by index {} in the json data:", index)) { }

std::ostream &operator<<(std::ostream &stream, const JsonException &instance) {
	return (stream << "Json object exception! " << instance._wrong_query << '\n' << instance._json_data);
}