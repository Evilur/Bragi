#pragma once

#include "type/radix_tree.h"
#include "util/class.h"
#include "web/http_transport.h"

#include <cstdint>
#include <cstring>

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

    bool _is_connection_close = false;

    const char* ReadHttp11ResponseLine(const char* buffer,
                                       uint32_t& buffer_size) noexcept;

    const char* ReadHttp11ResponseHeader(const char* buffer,
                                         uint32_t& buffer_size,
                                         bool& readingLongHeader) noexcept;

    const char* ReadHttp11LongHeader(const char* buffer,
                                     uint32_t& buffer_size,
                                     bool& readingLongHeader) noexcept;

private:
    enum Header {
        CONTENT_LENGTH,
        TRANSFER_ENCODING,
        CONNECTION,
        CONTENT_ENCODING
    };

    static const RadixTree<Header> _headers;

    template <uint32_t N>
    static constexpr auto HeadersToLower(
        KeyValuePair<const char*, Header> (&&entries)[N],
        String (&&scratches)[N] = { }
    ) noexcept -> const KeyValuePair<const char*, Header> (&)[N];
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
    bool readingLongHeader = false;

    /* While the connection is alive */
    do
        do
            if (!readingLongHeader)
                buffer = ReadHttp11ResponseHeader(buffer, buffer_size,
                                                  readingLongHeader);
            else
                buffer = ReadHttp11LongHeader(buffer, buffer_size,
                                              readingLongHeader);
        while(buffer != nullptr);
    while ((buffer_size = transport.Peek(buffer, HttpTransport::PEEK_MAX)) ==
           HttpTransport::PEEK_MAX);
}

inline const char* HttpResponse::ReadHttp11ResponseLine(
    const char* buffer, uint32_t& buffer_size
) noexcept {
    /* Http v1.1 first bytes */
    constexpr static const char http11_header[] = "HTTP/1.1 ";

    /* Check the min response line size */
    if (buffer_size < sizeof(http11_header) - 1 + 4 + 2) return nullptr;

    /* Check for the right http version */
    if (strncmp(buffer, http11_header, sizeof(http11_header) - 1) != 0)
        return nullptr;

    /* Skip the http version */
    buffer += sizeof(http11_header) - 1;
    buffer_size -= sizeof(http11_header) - 1;

    /* Read the response status code */
    _status_code = String::ToInt<uint16_t>(buffer);
    if (_status_code < 100 || _status_code > 599) {
        _status_code = 0;
        return nullptr;
    }

    /* Check for the space char */
    buffer += 4;
    buffer_size -= 4;
    if (buffer[-1] != ' ') return nullptr;

    /* Skip the rest of the response line */
    const char* buffer_eof =
        (const char*)memmem(buffer, buffer_size, "\r\n", 2);
    if (buffer_eof == nullptr) return nullptr;

    /* Return the pointer to the next line */
    buffer_size -= (buffer_eof - buffer) + 2;
    return buffer_eof + 2;
}

inline const char* HttpResponse::ReadHttp11ResponseHeader(
    const char* buffer, uint32_t& buffer_size, bool& readingLongHeader
) noexcept {
    fwrite(buffer, 1, buffer_size, stdout);
    return nullptr;
}

inline const char* HttpResponse::ReadHttp11LongHeader(
    const char* buffer, uint32_t& buffer_size, bool& readingLongHeader
) noexcept {
    return nullptr;
}

template <uint32_t N>
inline constexpr auto HttpResponse::HeadersToLower(
    KeyValuePair<const char*, Header> (&&entries)[N],
    String (&&scratches)[N]
) noexcept -> const KeyValuePair<const char*, Header> (&)[N] {
    for (uint32_t i = 0; i < N; ++i) {
        /* Save the entry key to the scratches and cast to the lowercase */
        (scratches[i] = entries[i].Key()).ToLower();

        /* Update the entry */
        entries[i] = { scratches[i].CStr(), entries[i].Value() };
    }

    /* Return the result */
    return entries;
}

inline constexpr RadixTree<HttpResponse::Header> HttpResponse::_headers {
    HeadersToLower({
        { "Content-Length"    , CONTENT_LENGTH      },
        { "Transfer-Encoding" , TRANSFER_ENCODING   },
        { "Connection"        , CONNECTION          },
        { "Content-Encoding"  , CONTENT_ENCODING    }
    })
};
