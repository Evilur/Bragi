#pragma once

#include "json_exception.h"
#include "util/logger.h"

#include <format>
#include <exception>

class JsonException final : std::exception {
public:
        explicit JsonException(const char* json_data, const char* query);

        explicit JsonException(const char* json_data, int index);
};

JsonException::JsonException(const char* const json_data, const char* query) {
        FATAL_LOG(std::format("Json object exception! Can't find the object by key \"{}\" in the json data:\n{}", query, json_data).c_str());
}

JsonException::JsonException(const char* const json_data, const int index) {
        FATAL_LOG(std::format("Json object exception! Can't find the object by index {} in the json data:\n{}", index, json_data).c_str());
}
