#pragma once

#include "type/dynamic_array.h"
#include "type/nullable.h"
#include "type/string.h"
#include "util/class.h"
#include "util/logger.h"
#include "http/http_transport.h"

/**
 * A simple representation of the http request
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpRequest final {
public:
    const String domain;
    const String method;
    bool close_connection = true;

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

    uint64_t ContentLength() const noexcept;

    bool IsChunkedEncoding() const noexcept;

    void SendHttp11Headers(HttpTransport& transport);

    void ResetContentLength() noexcept;

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

    Nullable<uint64_t> _content_length = nullptr;

    const String _path;

    void SaveHeader(const char* name, const char* value) noexcept;
};

inline HttpRequest::HttpRequest(const char* const domain,
                                const char* const path,
                                const char* const method) noexcept
    : domain(domain), _path(path), method(method) { }

inline HttpRequest&
HttpRequest::SetHeaders(const char* const (* const headers)[2],
                        const uint32_t headers_num) {
    /* Create a new dynamic array to replace the old one */
    _headers = DynamicArray<Header>(headers_num + 4);

    /* Drop special headers to their default values */
    close_connection = true;
    ResetContentLength();

    /* Loop through the headers */
    for (uint32_t i = 0; i < headers_num; ++i) {
        /* Get a key and a value */
        const char* const name = headers[i][0];
        const char* const value = headers[i][1];

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
        assert(key != nullptr && strpbrk(key, "\r\n:=") == nullptr);
        assert(value == nullptr || strpbrk(value, "\r\n;") == nullptr);

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
noexcept {
    /* Check the key and the value */
    assert(key != nullptr && strpbrk(key, "\r\n:=") == nullptr);
    assert(value == nullptr || strpbrk(value, "\r\n;") == nullptr);

    /* Save the cookie */
    _cookies.Push({ key, value });
    return *this;
}

inline HttpRequest& HttpRequest::SetContentLength(const uint64_t length)
noexcept { _content_length = length; return *this; }

inline uint64_t HttpRequest::ContentLength() const
noexcept {
    assert(_content_length != nullptr);
    return *_content_length;
}

inline bool HttpRequest::IsChunkedEncoding() const
noexcept { return _content_length == nullptr; }

inline void HttpRequest::SendHttp11Headers(HttpTransport& transport) {
    /* Send the request line */
    transport.RawWrite(method);
    transport.RawWrite(' ');
    transport.RawWrite(_path);
    transport.RawWrite(' ');
    transport.RawWrite("HTTP/1.1\r\n");

    /* Send a host header */
    transport.RawWrite("Host: ");
    transport.RawWrite(domain);
    transport.RawWrite("\r\n");

    /* Set the 'Content-Length' or 'Transfer-Encoding' header */
    if (_content_length == nullptr)
        transport.RawWrite("Transfer-Encoding: Chunked\r\n");
    else {
        transport.RawWrite("Content-Length: ");
        transport.RawWrite(String::Format("%llu", *_content_length));
        transport.RawWrite("\r\n");
    }

    /* Set the 'Connection' header according to the 'closeConnection' flag */
    transport.RawWrite("Connection: ");
    if (close_connection)
        transport.RawWrite("Close\r\n");
    else
        transport.RawWrite("Keep-Alive\r\n");

    /* Send cookies */
    if (!_cookies.IsEmpty()) {
        /* Send the cookie header name */
        transport.RawWrite("Cookie: ");

        /* Send all the cookies */
        for (uint32_t i = 0; i < _cookies.Size();) {
            Cookie& cookie = _cookies[i];
            transport.RawWrite(cookie.key);
            transport.RawWrite('=');
            transport.RawWrite(cookie.value);

            /* Send the delimeter */
            if (++i < _cookies.Size()) transport.RawWrite("; ");
        }

        /* Send the EOF sequence */
        transport.RawWrite("\r\n");
    }

    /* Send the other headers */
    if (!_headers.IsEmpty()) {
        for (uint32_t i = 0; i < _headers.Size(); ++i) {
            Header& header = _headers[i];
            transport.RawWrite(header.name);
            transport.RawWrite(": ");
            transport.RawWrite(header.value);

            /* Send the EOF sequence */
            transport.RawWrite("\r\n");
        }
    }

    /* Send yet another one EOF sequence (an empty line )
     * for the end of the headers */
    transport.RawWrite("\r\n");
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

inline void HttpRequest::ResetContentLength()
noexcept { _content_length = nullptr; }

inline void HttpRequest::SaveHeader(const char* const name,
                                    const char* const value) noexcept {
    /* Check for injections */
    assert(name != nullptr && strpbrk(name, "\r\n:=") == nullptr);
    assert(value != nullptr && strpbrk(value, "\r\n") == nullptr);

    /* Create a header */
    /* The header name is in the lower case now */
    Header header(name, value);

    /* Check for the host, cookie and transfer-encoding headers */
    assert(header.name != "host" &&
           header.name != "cookie" &&
           header.name != "transfer-encoding");

    /* Check for the 'Connection' header */
    if (header.name == "connection") {
        /* Check the value */
        header.value.ToLower();
        if (header.value == "close") close_connection = true;
        else if (header.value == "keep-alive") close_connection = false;
    }

    /* Check for the 'Content-Length' header
     * Set the content length */
    else if (header.name == "content-length")
        _content_length = String::ToInt<uint64_t>(header.value);

    /* If there is not a special header
     * Push the header to the array */
    else _headers.Push(move(header));
}
