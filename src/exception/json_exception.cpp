#include "json_exception.h"

JsonException::JsonException(const char* data, const char* query) : _data(data), _query(query) { }