#pragma once

#include "type/dynamic_array.h"
#include "type/string.h"
#include "util/class.h"

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

    void AddHeader(const char* name, const char* value) noexcept;

    void AddCookie(const char* key, const char* value) noexcept;

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

inline void HttpRequest::AddHeader(const char* const name,
                                   const char* const value)
noexcept { _headers.Push({ name, value }); }

inline void HttpRequest::AddCookie(const char* const key,
                                   const char* const value)
noexcept { _cookies.Push({ key, value }); }

inline HttpRequest::Header::Header(const char* const name,
                                   const char* const value) noexcept
    : name(name), value(value) { }

inline HttpRequest::Cookie::Cookie(const char* const key,
                                   const char* const value) noexcept
    : key(key), value(value) { }
