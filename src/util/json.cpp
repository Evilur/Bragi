#include "json.h"

#include <iostream>
#include <complex>

JSON::JSON(const char* data, const int &data_size) : _data(data), _data_size(data_size) { }

JSON JSON::Get(const char* query) const {
	throw std::logic_error("Not implemented yet");
}

const char* JSON::GetString(const char* query) const {
	return GetData(query);
}

const char* JSON::GetData(const char* query) const {
	unsigned short bracket_level = 0;  //Number of bracket levels
	bool in_string = false;  //true if caret in the string type; false if not

	return nullptr;

	int c = 0;
	while (*query != '\0') {
		std::cout << c++ << '\n';
		query = std::strchr(query, '.') + 1;
		const char* query_end = std::strchr(query, '.');

		std::cout << c++ << '\n';
	}

	return nullptr;
	int query_size = 0;
	for (int i = 0; i < _data_size && _data[i] != '\0'; i++) {
		/* Increase or descrease the bracket level var and compare the json to the key */
		if (!in_string) {
			if (_data[i] == '{') bracket_level++;
			else if (_data[i] == '}') bracket_level--;
			else if (bracket_level == 1 && CompareKey(&_data[i], query, query_size)) std::cout << &_data[i];
		}

		/* If we enter the string type or escape it, spawp the boolean */
		if (_data[i] == '\"' && _data[i - 1] != '\\') in_string ^= true;

		/* Exit the loop if the brackets level is 0 */
		if (bracket_level == 0) break;
	}
}

bool JSON::CompareKey(const char* json, const char* key, const int size) {
	/* First char is a quote */
	if (*json++ != '\"') return false;

	/* Compare json and a key */
	for (int i = 0; i < size; i++) if (*json++ != *key++) return false;

	/* Quote in the end of a key */
	if (*json++ != '\"') return false;

	/* Last char is a colon */
	return *json == ':';
}

int JSON::ToInt(const char* data, const int size) {
	if (data[0] != '-') return ToUnsignedInt(data, size);
	else return -ToUnsignedInt(++data, size);
}

int JSON::ToUnsignedInt(const char* data, int size) {
	int result = 0;
	for (int ten_pow = size - 1; ten_pow >= 0; ten_pow--)
		result += std::pow(10, ten_pow) * (*data++ - 48);  //48 is an index of the '0' char in the ASCII
	return result;
}