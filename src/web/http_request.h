#pragma once

#include "exception/http_request_exception.h"
#include "type/dynamic_array.h"
#include "type/string.h"
#include "util/class.h"
#include "util/logger.h"
#include "web/http_transport.h"

/**
 * A simple representation of the http request
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpRequest final {
public:
    const String domain;

    HttpRequest(const char* domain,
                const char* path,
                const char* method = "GET") noexcept;

    HttpRequest(const HttpRequest & other) = default;
    HttpRequest &operator=(const HttpRequest & other) = delete;
    HttpRequest(HttpRequest && other) noexcept = default;
    HttpRequest &operator=(HttpRequest &&) noexcept = delete;

    void SetHeaders(const char* const (*headers)[2],
                    uint32_t headers_num);

    void SetCookies(const char* const (*cookies)[2],
                    uint32_t cookies_num);

    void AddHeader(const char* name, const char* value) noexcept;

    void AddCookie(const char* key, const char* value) noexcept;

    void SendHttp11Headers(HttpTransport& transport) noexcept;

private:
    struct Header final {
        ALLOW_COPY_ALLOW_MOVE(Header);
        Header(const char* name, const char* value) noexcept;
        String name;
        String value;
    };

    struct Cookie final {
        ALLOW_COPY_ALLOW_MOVE(Cookie);
        Cookie(const char* key, const char* value) noexcept;
        String key;
        String value;
    };

    DynamicArray<Header> _headers { 8 };
    DynamicArray<Cookie> _cookies { 4 };

    const String _path;
    const String _method;
};

inline HttpRequest::HttpRequest(const char* const domain,
                                const char* const path,
                                const char* const method) noexcept
    : domain(domain), _path(path), _method(method) { }

inline void HttpRequest::SetHeaders(const char* const (* const headers)[2],
                                    const uint32_t headers_num) {
    /* Create a new dynamic array to replace the old one */
    DynamicArray<Header> new_headers(headers_num + 4);

    /* Loop through the headers */
    for (uint32_t i = 0; i < headers_num; ++i) {
        /* Get a key and a value */
        const char* const key = headers[i][0];
        const char* const value = headers[i][1];

        /* Check the key and the value */
        if (key == nullptr || value == nullptr) {
            ERROR_LOG("Failed to set the header: %s cannot be nullptr",
                      key == nullptr ? "key" : "value");
            throw HttpRequestException();
        }

        /* If all is OK, push the header to the array */
        new_headers.Push({ key, value });
    }

    /* Replace the old headers array with a new one */
    _headers = move(new_headers);
}

inline void HttpRequest::SetCookies(const char* const (* const cookies)[2],
                                    const uint32_t cookies_num) {
    /* Create a new dynamic array to replace the old one */
    DynamicArray<Cookie> new_cookies(cookies_num + 2);

    /* Loop through the cookies */
    for (uint32_t i = 0; i < cookies_num; ++i) {
        /* Get a key and a value */
        const char* const key = cookies[i][0];
        const char* const value = cookies[i][1];

        /* Check the key and the value */
        if (key == nullptr) {
            ERROR_LOG("Failed to set the cookie: key cannot be nullptr");
            throw HttpRequestException();
        }

        /* If all is OK, push the cookie to the array */
        new_cookies.Push({ key, value });
    }

    /* Replace the old cookies array with a new one */
    _cookies = move(new_cookies);
}

inline void HttpRequest::AddHeader(const char* const name,
                                   const char* const value)
noexcept { _headers.Push({ name, value }); }

inline void HttpRequest::AddCookie(const char* const key,
                                   const char* const value)
noexcept { _cookies.Push({ key, value }); }

inline void HttpRequest::SendHttp11Headers(HttpTransport& transport)
noexcept {
    transport.Send("PENIS", 5);
    transport.Flush();
}

inline HttpRequest::Header::Header(const char* const name,
                                   const char* const value) noexcept
    : name(name), value(value) {
    /* Cast the header to the lower case */
    this->name.ToLower();
}

inline HttpRequest::Cookie::Cookie(const char* const key,
                                   const char* const value) noexcept
    : key(key), value(value) { }
