#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#ifndef BRAGI_JSON_H
#define BRAGI_JSON_H

#include "http/http_client.h"

class JSON final {
public:
	explicit JSON(HttpClient &http_client);

	~JSON();

private:
	char* _data;
	int _data_size;
};

#endif
#pragma clang diagnostic pop