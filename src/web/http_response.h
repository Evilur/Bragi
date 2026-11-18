#pragma once

#include "type/uniq_ptr.h"
#include "util/class.h"
#include "util/minmax.h"
#include "web/http_transport.h"

#include <cstdint>

/**
 * A simple representation of the http response
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpResponse final {
public:
    PREVENT_COPY_ALLOW_MOVE(HttpResponse);
    HttpResponse() noexcept = default;

    bool IsValid() const noexcept;

    uint16_t StatusCode() const noexcept;

    bool IsChunkedEncoding() const noexcept;

    void ReadHttp11Headers(HttpTransport& transport) noexcept;

private:
    bool _is_valid = false;
    uint16_t _status_code = 0;

    uint64_t _content_length = 0;
    bool _is_chunked_encoding = false;

    const char* ReadHttp11ResponseLine(const char* buffer,
                                       uint32_t& buffer_size) noexcept;

    const char* ReadHttp11ResponseHeader(const char* buffer,
                                         uint32_t& buffer_size) noexcept;

    const char* ReadHttp11LongHeader(const char* buffer,
                                     uint32_t& buffer_size) noexcept;
};

inline bool HttpResponse::IsValid() const noexcept { return _is_valid; }

inline uint16_t HttpResponse::StatusCode() const
noexcept { return _status_code; }

inline bool HttpResponse::IsChunkedEncoding() const
noexcept { return _is_chunked_encoding; }

inline void HttpResponse::ReadHttp11Headers(HttpTransport& transport)
noexcept {
    /* Save the buffer pointer */
    const char* buffer;
    uint32_t buffer_size = transport.Peek(buffer, HttpTransport::PEEK_MAX);

    /* Check for the first peek */
    if (buffer_size == 0) return;

    /* Read the response line and check it */
    buffer = ReadHttp11ResponseLine(buffer, buffer_size);
    if (buffer == nullptr) return;

    /* Create e enum to save the reading state */
    enum : uint8_t {
        RESPONSE_HEADERS,
        LONG_HEADER
    } state = RESPONSE_HEADERS;

    /* While the connection is alive */
    do
        do
            if (state == RESPONSE_HEADERS)
                buffer = ReadHttp11ResponseLine(buffer, buffer_size);
            else
                buffer = ReadHttp11LongHeader(buffer, buffer_size);
        while(buffer != nullptr);
    while ((buffer_size = transport.Peek(buffer, HttpTransport::PEEK_MAX)) ==
           HttpTransport::PEEK_MAX);
}

inline const char* HttpResponse::ReadHttp11ResponseLine(
    const char* buffer, uint32_t& buffer_size
) noexcept {
    /* Http v1.1 first bytes */
    constexpr static const char http11_header[] = "HTTP/1.1";

    /* Check for the right http version */
    if (strncmp(buffer, http11_header,
                min(sizeof(http11_header) - 1, buffer_size)) != 0)
        return nullptr;

    /* Skip the http version */
    buffer += sizeof(http11_header);
    buffer_size -= sizeof(http11_header);

    /* Read the response status code */
    _status_code = String::ToInt<uint16_t>(buffer);
    if (_status_code < 100 || _status_code > 599) {
        _status_code = 0;
        return nullptr;
    }

    /* Check for the space char */
    buffer += 3;
    buffer_size -= 3;
    if (*buffer != ' ') return nullptr;

    /* Skip the rest of the response line */
    const char* buffer_eof =
        (const char*)memmem(buffer, buffer_size, "\r\n", 2);
    if (buffer_eof == nullptr) return nullptr;

    /* Return the pointer to the next line */
    return buffer + 2;
}

inline const char* HttpResponse::ReadHttp11ResponseHeader(
    const char* buffer, uint32_t& buffer_size
) noexcept {
    return nullptr;
}

inline const char* HttpResponse::ReadHttp11LongHeader(
    const char* buffer, uint32_t& buffer_size
) noexcept {
    return nullptr;
}
