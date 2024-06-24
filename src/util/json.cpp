#include <iostream>
#include "json.h"
#include "exception/json_exception.h"
#include "parser.h"
#include "logger.h"

Json::Json(const char* data) : _data(data) { }

Json Json::Get(const char* const key) const {
	const char* data_ptr = _data;
	if (!Find(data_ptr, key)) throw JsonException(_data, key);
	return Json(data_ptr);
}

Json Json::Get(const int index) const {
	const char* data_ptr = _data;
	if (!Find(data_ptr, index)) throw JsonException(_data, index);
	return Json(data_ptr);
}

Json Json::operator[](const char* key) const {
	const char* data_ptr = _data;
	if (!Find(data_ptr, key)) throw JsonException(_data, key);
	return Json(data_ptr);
}

Json Json::operator[](int index) const {
	const char* data_ptr = _data;
	if (!Find(data_ptr, index)) throw JsonException(_data, index);
	return Json(data_ptr);
}

bool Json::Has(const char* key) const {
	const char* data_ptr = _data;
	return Find(data_ptr, key);
}

bool Json::Has(const int index) const {
	const char* data_ptr = _data;
	return Find(data_ptr, index);
}

bool Json::IsEmpty() const { return (*_data == '{' && *(_data + 1) == '}') || (*_data == '[' && *(_data + 1) == ']'); }

Json::operator std::string() const {
	const char* data_ptr = _data + 1;
	const char* data_end = std::strchr(data_ptr, '\"');

	std::stringstream result;

	do {
		/* If the current char is not a backslash or the next char is not the 'u' */
		if (*data_ptr != '\\' || *(data_ptr + 1) != 'u') result.put(*data_ptr);
		else {
			result << Parser::Utf8(data_ptr);
			data_ptr += 5;
		}
	} while (++data_ptr < data_end);

	return result.str();
}

Json::operator unsigned short() const { return Parser::ToUInt16(_data); }

Json::operator short() const { return Parser::ToInt16(_data); }

Json::operator unsigned int() const { return Parser::ToUInt32(_data); }

Json::operator int() const { return Parser::ToInt32(_data); }

Json::operator unsigned long() const { return Parser::ToUInt64(_data); }

Json::operator long() const { return Parser::ToInt64(_data); }


bool Json::Find(const char*&data, const char* key) {
	unsigned short square_bracket_level = 0;  //Number of square bracket levels
	unsigned short curly_bracket_level = 0;  //Number of curly bracket levels
	bool in_string = false;  //true if caret in the string type; false if not

	do {
		if (!in_string) {
			/* Increase or descrease the bracket level var and compare the json to the key */
			if (*data == '{') curly_bracket_level++;
			else if (*data == '}') curly_bracket_level--;
			else if (*data == '[') square_bracket_level++;
			else if (*data == ']') square_bracket_level--;
			else if (curly_bracket_level == 1 && CompareKey(data, key)) return data = std::strchr(data, ':') + 1;
		}

		/* If we enter the string type or escape it, spawp the boolean */
		if (*data == '\"' && *(data - 1) != '\\') in_string ^= true;

		/* Increase the pointer */
		data++;
	} while (curly_bracket_level > 0);

	return false;
}

bool Json::Find(const char*&data, int index) {
	unsigned short square_bracket_level = 0;  //Number of square bracket levels
	unsigned short curly_bracket_level = 0;  //Number of curly bracket levels
	bool in_string = false;  //true if caret in the string type; false if not

	/* If there is a first object in the array */
	if (index == 0) return ++data;

	do {
		if (!in_string) {
			/* Increase or descrease the bracket level var and compare the json to the key */
			if (*data == '{') curly_bracket_level++;
			else if (*data == '}') curly_bracket_level--;
			else if (*data == '[') square_bracket_level++;
			else if (*data == ']') square_bracket_level--;
			else if (*data == ',' && curly_bracket_level == 0 && square_bracket_level == 1 && --index == 0) return ++data;
		}

		/* If we enter the string type or escape it, spawp the boolean */
		if (*data == '\"' && *(data - 1) != '\\') in_string ^= true;

		/* Increase the pointer */
		data++;
	} while (square_bracket_level > 0);

	return false;
}

bool Json::CompareKey(const char* data, const char* key) {
	/* First char is a quote */
	if (*data++ != '\"') return false;

	/* Compare json and a key */
	while (*key != '\0') if (*data++ != *key++) return false;

	/* Quote in the end of a key */
	if (*data++ != '\"') return false;

	/* Last char is a colon */
	return *data == ':';
}