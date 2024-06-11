#include "json_exception.h"

JsonException::JsonException(const char* data, const char* query) : _data(data), _query(query), _index(-1) { }

JsonException::JsonException(const char* data, const int index) : _data(data), _query(nullptr), _index(index) { }