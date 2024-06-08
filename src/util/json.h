#ifndef BRAGI_JSON_H
#define BRAGI_JSON_H

#include "http/http_client.h"

class Json final {
public:
	explicit Json(const char* data);

	Json GetObject(const char* query) const;

	const char* GetString(const char* query) const;

	unsigned int GetUInt(const char* query) const;

private:
	const char* const _data;

	const char* GetData(const char* query) const;

	static bool FindObject(const char*&data, const char* key, const char* key_end);

	static bool CompareKey(const char* data, const char* key, const char* key_end);

	static int ToInt(const char* data);

	static int ToUnsignedInt(const char* data);
};

#endif