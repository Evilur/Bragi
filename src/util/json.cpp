#include "json.h"

#include <iostream>
#include <complex>

JSON::JSON(const char* data) : _data(data) { }

JSON JSON::Get(const char* query) const {
	throw std::logic_error("Not implemented yet");
}

const char* JSON::GetString(const char* query) const {
	return GetData(query);
}

const char* JSON::GetData(const char* query) const {
	unsigned short bracket_level;  //Number of bracket levels
	bool in_string;  //true if caret in the string type; false if not

	while (*query != '\0') {
		const char* end_ptr;
		bracket_level = 0;

		/* If this iteration is an array acessing */
		if (*query++ == '[') {
			end_ptr = std::strchr(query, ']') + 1;
			const unsigned int index = ToUnsignedInt(query);
		} else {
			end_ptr = std::strchr(query, '.');  //Get index of a dot char
			if (!end_ptr) end_ptr = std::strchr(query, '\0'); //If there isn't any dot chars get the null char ptr
			if (*(end_ptr - 1) == ']') end_ptr = std::strchr(query, '[');  //If there is a second square braket the index of the first one
			const int query_size = end_ptr - query; //Get the size of the query

			while (bracket_level == 0 && *query++ != '\n') {

			}
		}

		query = end_ptr;  //Go the next key
	}

	return nullptr;
	int query_size = 0;
	for (int i = 0; i < 1 && _data[i] != '\0'; i++) {
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

int JSON::ToInt(const char* data) {
	if (data[0] != '-') return ToUnsignedInt(data);
	else return -ToUnsignedInt(data + 1);
}

int JSON::ToUnsignedInt(const char* data) {
	int result = 0;
	while (*data >= '0' and *data <= '9') result = result * 10 + (*data++ - '0');
	return result;
}