#ifndef BRAGI_JSON_H
#define BRAGI_JSON_H

#include "web/http_client.h"

/** Class for working with the JSON data
 * @warning Can work with minified JSON data only (without any spaces and new line characters)
 */
class Json final {
public:
	explicit Json(const char* data);

	Json operator[](const char* key) const;

	Json operator[](int index) const;

	bool Has(const char* key) const;

	bool Has(int index) const;

	bool IsEmpty() const;

	explicit operator std::string() const;

	explicit operator const char*() const;

	explicit operator unsigned short() const;

	explicit operator short() const;

	explicit operator unsigned int() const;

	explicit operator int() const;

	explicit operator unsigned long() const;

	explicit operator long() const;

private:
	const char* const _data;

	static bool Find(const char*&data, const char* key);

	static bool Find(const char*&data, int index);

	static bool CompareKey(const char* data, const char* key);
};

#endif