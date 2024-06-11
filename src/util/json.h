#ifndef BRAGI_JSON_H
#define BRAGI_JSON_H

#include "http/http_client.h"

/** Class for working with the JSON data
 * @warning Can work with minified JSON data only (without any spaces and new line characters)
 */
class Json final {
public:
	explicit Json(const char* data);

	Json Get(const char* key) const;

	Json Get(int index) const;

	explicit operator std::string() const;

	explicit operator unsigned int() const;

	explicit operator int() const;

private:
	const char* const _data;

	static bool Find(const char*&data, const char* key);

	static bool Find(const char*&data, int index);

	static bool CompareKey(const char* data, const char* key);

	static int ToInt(const char* data);

	static int ToUnsignedInt(const char* data);
};

#endif