#include "json.h"
#include "exception/json_exception.h"

#include <iostream>

Json::Json(const char* data) : _data(data) { }

Json Json::GetObject(const char* const query) const {
	return Json(GetData(query));
}

std::string Json::GetString(const char* const query) const {
	const char* data = GetData(query) + 1;
	const int data_size = std::strchr(data, '\"') - data;

	std::string result(data, data_size);
	std::erase(result, '\\');
	return result;
}

unsigned int Json::GetUInt(const char* const query) const {
	return ToUnsignedInt(GetData(query));
}

int Json::GetInt(const char* const query) const {
	return ToInt(GetData(query));
}

const char* Json::GetData(const char* const query) const {
	const char* data_ptr = _data;
	const char* key = query;

	while (*key != '\0') {
		const char* query_end = nullptr;  //Pointer to the end of a current key str

		/* If this iteration is an array acessing */
		if (*key++ == '[') {
			query_end = std::strchr(key, ']') + 1;
			const unsigned int index = ToUnsignedInt(key);
			if (!FindObject(data_ptr, index)) throw JsonException(_data, query);
		} else {
			query_end = std::strchr(key, '.');  //Get index of a dot char
			if (!query_end) query_end = std::strchr(key, '\0'); //If there isn't any dot chars get the null char ptr
			if (*(query_end - 1) == ']') query_end = std::strchr(key, '[');  //If there is a second square braket the index of the first one
			if (!FindObject(data_ptr, key, query_end)) throw JsonException(_data, query);  //If we cannot find the object throw the exception
		}

		key = query_end;  //Go the next key
	}

	return data_ptr;
}

bool Json::FindObject(const char*&data, const char* key, const char* key_end) {
	unsigned short bracket_level = 0;  //Number of bracket levels
	bool in_string = false;  //true if caret in the string type; false if not

	do {
		if (!in_string) {
			/* Increase or descrease the bracket level var and compare the json to the key */
			if (*data == '{') bracket_level++;
			else if (*data == '}') bracket_level--;
			else if (bracket_level == 1 && CompareKey(data, key, key_end)) return data += key_end - key + 3;
		}

		/* If we enter the string type or escape it, spawp the boolean */
		if (*data == '\"' && *(data - 1) != '\\') in_string ^= true;

		/* Increase the pointer */
		data++;
	} while (bracket_level > 0);

	return false;
}

bool Json::FindObject(const char*&data, unsigned int index) {
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

	std::cout << 3123123123;

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