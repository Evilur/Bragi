#include "json.h"

#include <iostream>

JSON::JSON(HttpClient &http_client) {
	_data_size = 1024 * 8;
	_data = new char[_data_size];
	http_client.ReadAll(_data, _data_size, _data_size);
}

JSON::~JSON() {
	delete[] _data;
	_data = nullptr;
}