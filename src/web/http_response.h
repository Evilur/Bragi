#pragma once

#include "reflection/enum.h"
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
    enum Header : uint8_t {
        CONNECTION,
        CONTENT_ENCODING,
        CONTENT_LENGTH,
        TRAILER,
        TRANSFER_ENCODING
    };

    enum ParseResult : uint8_t {
        NEXT_LINE,
        NEED_MORE,
        HEADERS_END,
        INVALID
    };

    bool _is_valid = false;
    uint16_t _status_code = 0;

    uint64_t _content_length = 0;
    bool _is_chunked_encoding = false;

    bool _is_connection_close = false;

    ParseResult ReadHttp11ResponseLine(const char*& buffer,
                                       uint32_t& buffer_size) noexcept;

    ParseResult ReadHttp11ResponseHeader(const char*& buffer,
                                         uint32_t& buffer_size) noexcept;

    ParseResult ReadHttp11LongHeader(const char*& buffer,
                                     uint32_t& buffer_size) noexcept;

private:
    template <uint32_t N>
    static constexpr auto HeadersToLower(
        KeyValuePair<const char*, Header> (&&entries)[N],
        String (&&scratches)[N] = { }
    ) noexcept -> const KeyValuePair<const char*, Header> (&)[N];

    static constexpr char LazyValidateHeader(const char*& buffer) noexcept;

    static constexpr bool IsTextChar(unsigned char chr) noexcept;

    static constexpr bool IsObsTextChar(unsigned char chr) noexcept;

    static constexpr bool IsTokenChar(unsigned char chr) noexcept;

    static constexpr ParseResult SkipHeaderName(
        const char*& buffer, uint32_t& buffer_size
    ) noexcept;

    static constexpr ParseResult SkipHeaderValue(
        const char*& buffer, uint32_t& buffer_size
    ) noexcept;

    static const RadixTree<Header> _headers;
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

    /* While the connection is alive, read the headers */
    do {
        do {
            /* Check for the minimum header size */
            if (buffer_size < 2) { buffer = nullptr; continue; }

            if (!readingLongHeader) {
                /* Check for the empty line (headers end) */
                if (memcmp(buffer, "\r\n", 2) == 0) {
                    buffer = nullptr; goto after_headers_reading;
                }

                buffer = ReadHttp11ResponseHeader(
                    buffer, buffer_size, readingLongHeader
                );
            } else
                buffer = ReadHttp11LongHeader(
                    buffer, buffer_size, readingLongHeader
                );
        } while(buffer != nullptr);
    } while ((buffer_size =
             transport.Peek(buffer, HttpTransport::PEEK_MAX - buffer_size)) ==
             HttpTransport::PEEK_MAX);
after_headers_reading:
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
    const char* const buffer, uint32_t& buffer_size, bool& readingLongHeader
) noexcept {
    /* Try to read the header */
    const Header* const header = _headers.Get<LazyValidateHeader>(buffer);

    /* Get the buffer pointer */
    const char* buffer_ptr;

    /* If it is a supported header */
    if (header != nullptr) {
        /* Set the buffer pointer after the space char + trim the space char */
        buffer_ptr = String::Trim(buffer + get_enum_label_length(*header) + 1);
        if (buffer_ptr > buffer + buffer_size) return nullptr;
    }

    /* If it is NOT a supported header */
    else {
        /* Skip the header name and the colon */
        buffer_ptr = SkipHeaderName(buffer, buffer_size);
        if (buffer_ptr == nullptr) return nullptr;

        /* Skip the header value */
        buffer_ptr =
            SkipHeaderValue(buffer_ptr, buffer_size, readingLongHeader);
    }

    /* If the header is invalid or not full in the buffer */
    if (buffer_ptr == nullptr) return nullptr;

    /* If all is OK, the buffer_ptr must point to the necx line */
    return buffer_ptr;
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

inline constexpr char HttpResponse::LazyValidateHeader(const char*& buffer)
noexcept {
    /* Get the char and increase the pointer */
    char chr = *buffer;
    ++buffer;

    /* Cast to lowercase */
    if (chr >= 'A' && chr <= 'Z')
        chr += 'a' - 'A';

    /* Use a colon as a terminate char */
    else if (chr == ':')
        chr = '\0';

    /* Return the result */
    return chr;
}

inline constexpr bool HttpResponse::IsTextChar(const unsigned char chr)
noexcept { return (chr >= 0x20 && chr <= 0x7e) || chr == '\t'; }

inline constexpr bool HttpResponse::IsObsTextChar(const unsigned char chr)
noexcept { return chr >= 0x80; }

inline constexpr bool HttpResponse::IsTokenChar(const unsigned char chr)
noexcept {
    if ((chr >= 'a' && chr <= 'z') ||
        (chr >= 'A' && chr <= 'Z') ||
        (chr >= '0' && chr <= '9'))
        return 1;

    switch (chr) {
        case '!': case '#': case '$': case '%': case '&': case '\'':
        case '*': case '+': case '-': case '.': case '^': case '_':
        case '`': case '|': case '~': return true;
        default: return false;
    }
}

inline constexpr const char* HttpResponse::SkipHeaderName(
    const char* const buffer, uint32_t& buffer_size
) noexcept {
    /* Get the buffer pointer and the buffer end */
    const char* buffer_ptr = buffer;
    const char* const buffer_end = buffer + buffer_size;

    /* Skip all token chars */
    while (buffer_ptr < buffer_end && IsTokenChar(*buffer_ptr))
        ++buffer_ptr;

    /* The next char must be a colon
     * If that is false, or if there is no token chars before it,
     * Return nullptr */
    if (buffer_ptr == buffer || buffer_ptr == buffer_end || *buffer_ptr != ':')
        return nullptr;

    /* Skip the colon */
    ++buffer_ptr;

    /* Update the the size and return the pointer */
    buffer_size = buffer_end - buffer_ptr;
    return buffer_ptr;
}

inline constexpr const char* HttpResponse::SkipHeaderValue(
    const char* const buffer, uint32_t& buffer_size, bool& readingLongHeader
) noexcept {
    /* Check all the header value bytes in a loop */
    for (uint32_t i = 0; i < buffer_size; ++i) {
        /* Get the current byte */
        const unsigned char chr = buffer[i];

        /* Check for \r char */
        if (chr == '\r') {
            /* If there is not a eof (\r\n) sequence, return nullptr */
            if (i + 1 >= buffer_size || buffer[i + 1] != '\n') {
                readingLongHeader = false;
                return nullptr;
            }

            /* If there is a eof sequence,
             * Update the size and return the pointer to the next line */
            const uint32_t offset = i + 2;
            buffer_size  -= offset;
            readingLongHeader = false;
            return buffer + offset;
        }

        /* Check for all visible english chars + obsolute text */
        if (!IsTextChar(chr) && !IsObsTextChar(chr)) {
            readingLongHeader = false;
            return nullptr;
        }
    }

    /* If there is not the end of this header */
    readingLongHeader = true;
    buffer_size = 0;
    return nullptr;
}

inline constexpr RadixTree<HttpResponse::Header> HttpResponse::_headers {
    HeadersToLower({
        { "Connection"        , CONNECTION          },
        { "Content-Encoding"  , CONTENT_ENCODING    },
        { "Content-Length"    , CONTENT_LENGTH      },
        { "Trailer"           , TRAILER             },
        { "Transfer-Encoding" , TRANSFER_ENCODING   },
    })
};
