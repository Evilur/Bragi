#include "http_client_exception.h"
#include "util/dictionary.h"

WebClientException::WebClientException(const HttpClientErrorCode e_code) : _e_code(e_code), _e_data(GetData(e_code)) { }

const char* WebClientException::GetData(const HttpClientErrorCode e_code) {
	switch (e_code) {
		case CON_CANNOT_BE_ESTABLISHED:
			return DIC_HTTP_ERROR_CON_CANNOT_BE_ESTABLISHED;
	}
	return nullptr;
}

HttpClientErrorCode WebClientException::GetCode() { return _e_code; }

const char* WebClientException::GetData() { return _e_data; }
