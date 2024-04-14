#include "http_client_exception.h"
#include "util/dictionary.h"

HttpClientException::HttpClientException(const HttpClientErrorCode e_code) : _e_code(e_code), _e_data(GetData(e_code)) { }

const char* HttpClientException::GetData(const HttpClientErrorCode e_code) {
	switch (e_code) {
		case CON_CANNOT_BE_ESTABLISHED:
			return DIC_HTTP_ERROR_CON_CANNOT_BE_ESTABLISHED;
	}
	return nullptr;
}

HttpClientErrorCode HttpClientException::GetCode() { return _e_code; }

const char* HttpClientException::GetData() { return _e_data; }
