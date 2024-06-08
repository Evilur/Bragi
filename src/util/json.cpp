#include "json.h"
#include "exception/json_exception.h"

#include <iostream>
#include <complex>

Json::Json(const char* data) : _data(data) { }

Json Json::GetObject(const char* query) const {
	throw std::logic_error("Not implemented yet");
}

const char* Json::GetString(const char* query) const {
	return GetData(query);
}

unsigned int Json::GetUInt(const char* query) const {
	return ToUnsignedInt(GetData(query));
}

const char* Json::GetData(const char* query) const {
	const char* data_ptr = _data;

	while (*query != '\0') {
		const char* query_end = nullptr;  //Pointer to the end of a current key str

		/* If this iteration is an array acessing */
		if (*query++ == '[') {
			query_end = std::strchr(query, ']') + 1;
			const unsigned int index = ToUnsignedInt(query);
			throw std::logic_error("Not implemented yet");
		} else {
			query_end = std::strchr(query, '.');  //Get index of a dot char
			if (!query_end) query_end = std::strchr(query, '\0'); //If there isn't any dot chars get the null char ptr
			if (*(query_end - 1) == ']') query_end = std::strchr(query, '[');  //If there is a second square braket the index of the first one
			if (!FindObject(data_ptr, query, query_end)) throw JsonException(_data);  //If we cannot find the object throw the exception
		}

		query = query_end;  //Go the next key
	}

	return data_ptr;
	int query_size = 0;
	for (int i = 0; i < 1 && _data[i] != '\0'; i++) {
		/* Increase or descrease the bracket level var and compare the json to the key */
		/*if (!in_string) {
			if (_data[i] == '{') bracket_level++;
			else if (_data[i] == '}') bracket_level--;
			else if (bracket_level == 1 && CompareKey(&_data[i], query, query_size)) std::cout << &_data[i];
		}*/

		/* If we enter the string type or escape it, spawp the boolean */
		/*if (_data[i] == '\"' && _data[i - 1] != '\\') in_string ^= true;*/

		/* Exit the loop if the brackets level is 0 */
		/*if (bracket_level == 0) break;*/
	}
}

bool Json::FindObject(const char*&data, const char* key, const char* key_end) {
	unsigned short bracket_level = 0;  //Number of bracket levels
	bool in_string = false;  //true if caret in the string type; false if not

	do {
		if (!in_string) {
			/* Increase or descrease the bracket level var and compare the json to the key */
			if (*data == '{') bracket_level++;
			else if (*data == '}') bracket_level--;
			else if (bracket_level == 1 && CompareKey(data, key, key_end)) {
				data += key_end - key + 3;
				return true;
			}
		}

		/* If we enter the string type or escape it, spawp the boolean */
		if (*data == '\"' && *(data - 1) != '\\') in_string ^= true;

		/* Increase the pointer */
		data++;
	} while (bracket_level > 0);

	return false;
}

bool Json::CompareKey(const char* data, const char* key, const char* key_end) {
	/* First char is a quote */
	if (*data++ != '\"') return false;

	/* Compare json and a key */
	while (key < key_end) if (*data++ != *key++) return false;

	/* Quote in the end of a key */
	if (*data++ != '\"') return false;

	/* Last char is a colon */
	return *data == ':';
}

int Json::ToInt(const char* data) {
	if (data[0] != '-') return ToUnsignedInt(data);
	else return -ToUnsignedInt(data + 1);
}

int Json::ToUnsignedInt(const char* data) {
	int result = 0;
	while (*data >= '0' and *data <= '9') result = result * 10 + (*data++ - '0');
	return result;
}