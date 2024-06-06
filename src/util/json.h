#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#ifndef BRAGI_JSON_H
#define BRAGI_JSON_H

#include "http/http_client.h"

class JSON final {
public:
	JSON(const char* data, const int &data_size);

	JSON Get(const char* query) const;

	const char* GetString(const char* query) const;

private:
	const char* _data;
	const int _data_size;

	const char* GetData(const char* query) const;

	static bool CompareKey(const char* json, const char* key, int size);

	static int ToInt(const char* data, int size);

	static int ToUnsignedInt(const char* data, int size);
};

#endif
#pragma clang diagnostic pop