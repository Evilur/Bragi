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
    bool closeConnection = true;

    HttpRequest(const char* domain,
                const char* path,
                const char* method = "GET") noexcept;

    HttpRequest(const HttpRequest & other) = default;
    HttpRequest &operator=(const HttpRequest & other) = delete;
    HttpRequest(HttpRequest && other) noexcept = default;
    HttpRequest &operator=(HttpRequest &&) noexcept = delete;

    HttpRequest& SetHeaders(const char* const (*headers)[2],
                            uint32_t headers_num);

    HttpRequest& SetCookies(const char* const (*cookies)[2],
                            uint32_t cookies_num);

    HttpRequest& AddHeader(const char* name, const char* value) noexcept;

    HttpRequest& AddCookie(const char* key, const char* value) noexcept;

    HttpRequest& SetContentLength(uint64_t length) noexcept;

    HttpRequest& ResetContentLength() noexcept;

    bool IsChunkedEncoding() const noexcept;

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

    String _contentLength { 0 };

    const String _path;
    const String _method;

    void SaveHeader(const char* name, const char* value) noexcept;
};

inline HttpRequest::HttpRequest(const char* const domain,
                                const char* const path,
                                const char* const method) noexcept
    : domain(domain), _path(path), _method(method) { }

inline HttpRequest&
HttpRequest::SetHeaders(const char* const (* const headers)[2],
                        const uint32_t headers_num) {
    /* Create a new dynamic array to replace the old one */
    _headers = DynamicArray<Header>(headers_num + 4);

    /* Drop special headers to their default values */
    closeConnection = true;
    ResetContentLength();

    /* Loop through the headers */
    for (uint32_t i = 0; i < headers_num; ++i) {
        /* Get a key and a value */
        const char* const name = headers[i][0];
        const char* const value = headers[i][1];

        /* Check the name and the value */
        if (name == nullptr || value == nullptr) {
            ERROR_LOG("Failed to set the header: %s cannot be nullptr",
                      name == nullptr ? "name" : "value");
            throw HttpRequestException();
        }

        /* If all is OK, save the header */
        SaveHeader(name, value);
    }

    /* Return the reference to themselve */
    return *this;
}

inline HttpRequest&
HttpRequest::SetCookies(const char* const (* const cookies)[2],
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

    /* Return the reference to themselve */
    return *this;
}

inline HttpRequest& HttpRequest::AddHeader(const char* const name,
                                           const char* const value)
noexcept { SaveHeader(name, value); return *this; }

inline HttpRequest& HttpRequest::AddCookie(const char* const key,
                                           const char* const value)
noexcept { _cookies.Push({ key, value }); return *this; }

inline HttpRequest& HttpRequest::ResetContentLength() noexcept {
    if (!_contentLength.IsEmpty()) _contentLength = String(0);
    return *this;
}

inline HttpRequest& HttpRequest::SetContentLength(uint64_t length)
noexcept { _contentLength = String::Format("%llu", length); return *this; }

inline bool HttpRequest::IsChunkedEncoding() const
noexcept { return _contentLength.IsEmpty(); }

inline void HttpRequest::SendHttp11Headers(HttpTransport& transport)
noexcept {
    /* Send the request line */
    transport.Send(_method);
    transport.Send(' ');
    transport.Send(_path);
    transport.Send(' ');
    transport.Send("HTTP/1.1\r\n");

    /* Send a host header */
    transport.Send("Host: ");
    transport.Send(domain);
    transport.Send("\r\n");

    /* Set the 'Content-Length' or 'Transfer-Encoding' header */
    if (IsChunkedEncoding())
        transport.Send("Transfer-Encoding: chunked\r\n");
    else {
        transport.Send("Content-Length: ");
        transport.Send(_contentLength);
        transport.Send("\r\n");
    }

    /* Set the 'Connection' header according to the 'closeConnection' flag */
    transport.Send("Connection: ");
    if (closeConnection) transport.Send("close\r\n");
    else transport.Send("keep-alive\r\n");

    /* Send cookies */
    if (!_cookies.IsEmpty()) {
        /* Send the cookie header name */
        transport.Send("Cookie: ");

        /* Send all the cookies */
        for (uint32_t i = 0; i < _cookies.Size();) {
            Cookie& cookie = _cookies[i];
            transport.Send(cookie.key);
            transport.Send('=');
            transport.Send(cookie.value);

            /* Send the delimeter */
            if (++i < _cookies.Size()) transport.Send("; ");
        }

        /* Send the EOF sequence */
        transport.Send("\r\n");
    }

    /* Send the other headers */
    if (!_headers.IsEmpty()) {
        for (uint32_t i = 0; i < _headers.Size(); ++i) {
            Header& header = _headers[i];
            transport.Send(header.name);
            transport.Send(": ");
            transport.Send(header.value);

            /* Send the EOF sequence */
            transport.Send("\r\n");
        }
    }

    /* Send yet another one EOF sequence (an empty line )
     * for the end of the headers */
    transport.Send("\r\n");

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

inline void HttpRequest::SaveHeader(const char* const name,
                                    const char* const value) noexcept {
    /* Create a header */
    Header header(name, value);

    /* Check for the 'Connection' header */
    if (header.name == "connection") {
        /* Check the value */
        header.value.ToLower();
        if (header.value == "close") closeConnection = true;
        else if (header.value == "keep-alive") closeConnection = false;
    }

    /* Check for the 'Content-Length' header
     * Set the content length */
    else if (header.name == "content-length")
        _contentLength = header.value;

    /* If there is not a special header
     * Push the header to the array */
    else
        _headers.Push(move(header));
}
