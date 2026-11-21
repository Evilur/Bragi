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
        VALID,
        NEED_MORE,
        HEADERS_END,
        INVALID
    };

    bool _is_valid = false;
    uint16_t _status_code = 0;

    uint64_t _content_length = 0;
    bool _is_chunked_encoding = false;

    bool _is_connection_close = false;

    [[nodiscard]]
    const char* ReadHttp11ResponseLine(
        const char* buffer, const char* buffer_end
    ) noexcept;

    [[nodiscard]]
    ParseResult ReadHttp11ResponseHeader(
        const char*& buffer, const char* buffer_end, bool& readingLongHeader
    ) noexcept;

    [[nodiscard]]
    ParseResult ReadHttp11LongHeader(
        const char*& buffer, const char* buffer_end, bool& readingLongHeader
    ) noexcept;

private:
    template <uint32_t N>
    [[nodiscard]]
    static constexpr auto HeadersToLower(
        KeyValuePair<const char*, Header> (&&entries)[N],
        String (&&scratches)[N] = { }
    ) noexcept -> const KeyValuePair<const char*, Header> (&)[N];

    [[nodiscard]]
    static constexpr char LazyValidateHeader(const char*& buffer) noexcept;

    [[nodiscard]]
    static constexpr bool IsTextChar(unsigned char chr) noexcept;

    [[nodiscard]]
    static constexpr bool IsObsTextChar(unsigned char chr) noexcept;

    [[nodiscard]]
    static constexpr bool IsTokenChar(unsigned char chr) noexcept;

    [[nodiscard]]
    static constexpr ParseResult SkipHeaderName(
        const char*& buffer, const char* buffer_end
    ) noexcept;

    [[nodiscard]]
    static constexpr ParseResult SkipHeaderValue(
        const char*& buffer, const char* buffer_end, bool& readingLongHeader
    ) noexcept;

    [[nodiscard]]
    static constexpr ParseResult HandleHeaderValue(
        Header header, const char*& buffer,
        const char* buffer_end, bool& readingLongHeader
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
    uint32_t peeked = transport.Peek(buffer, HttpTransport::PEEK_MAX);
    if (peeked == 0) return;
    const char* buffer_end = buffer + peeked;

    /* Read the response line and check it */
    buffer = ReadHttp11ResponseLine(buffer, buffer_end);
    if (buffer == nullptr) return;

    /* To read long headers below */
    bool readingLongHeader = false;

    /* Read the headers */
    for (;;) {
        /* While we have what to read, read it (headers) */
        ParseResult result;
        do result = readingLongHeader
            ? ReadHttp11LongHeader(buffer, buffer_end, readingLongHeader)
            : ReadHttp11ResponseHeader(buffer, buffer_end, readingLongHeader);
        while (result == ParseResult::VALID);

        /* Consume the handled data from the buffer */
        const uint32_t handled = peeked - (buffer_end - buffer);
        transport.Consume(handled);

        /* Check the result */
        switch (result) {
            case ParseResult::HEADERS_END: _is_valid = true; return;
            case ParseResult::INVALID: return;
            default: break;
        }

        /* If we've NOT handled any bytes, and the buffer is full */
        if (handled == 0 && peeked == HttpTransport::PEEK_MAX) return;

        /* Peek a new data */
        peeked = transport.Peek(buffer, HttpTransport::PEEK_MAX);
        if (peeked == 0) return;
        buffer_end = buffer + peeked;
    }
}

inline const char* HttpResponse::ReadHttp11ResponseLine(
    const char* buffer, const char* const buffer_end
) noexcept {
    /* Http v1.1 first bytes */
    constexpr static const char http11_header[] = "HTTP/1.1 ";

    /* Check the min response line size */
    if (buffer_end - buffer < sizeof(http11_header) - 1 + 4 + 2)
        return nullptr;

    /* Check for the right http version */
    if (!String::Equal(buffer, http11_header, sizeof(http11_header) - 1))
        return nullptr;

    /* Skip the http version */
    buffer += sizeof(http11_header) - 1;

    /* Read the response status code */
    _status_code = String::ToInt<uint16_t>(buffer);
    if (_status_code < 100 || _status_code > 599) {
        _status_code = 0;
        return nullptr;
    }

    /* Skip the response status code */
    buffer += 3;

    /* Check for the right end of the status code */
    if (*buffer != ' ' && *buffer != '\r') return nullptr;

    /* Read and check the rest of the response line */
    while (buffer < buffer_end) {
        if (buffer[0] == '\r') {
            /* If there is \r\n seq, return the new pointer to the new line */
            if (buffer + 1 < buffer_end && buffer[1] == '\n')
                return buffer + 2;

            /* If there is only \r char, return nullptr */
            else return nullptr;
        }

        /* Check for the acceptable chars */
        if (!IsTextChar(*buffer)) return nullptr;

        /* Update the pointer */
        ++buffer;
    }

    /* If there is no line eof */
    return nullptr;
}

inline HttpResponse::ParseResult HttpResponse::ReadHttp11ResponseHeader(
    const char*& buffer, const char* const buffer_end, bool& readingLongHeader
) noexcept {
    /* Check for the buffer pointer */
    if (buffer >= buffer_end) return NEED_MORE;

    /* Check for the line eof sequence (\r\n) */
    if (buffer[0] == '\r') {
        if (buffer + 1 >= buffer_end) return NEED_MORE;
        if (buffer[1] != '\n') return INVALID;
        buffer += 2;
        return HEADERS_END;
    }

    /* Try to read the header */
    const Header* const header = _headers.Get<LazyValidateHeader>(buffer);

    /* Create a buffer pointer */
    const char* buffer_ptr = buffer;

    /* Create a result variable */
    ParseResult result;

    /* If it is a supported header */
    if (header != nullptr) {
        /* Set the buffer pointer after the colon char */
        buffer_ptr += get_enum_label_length(*header) + 1;

        /* Check for the remaining space */
        if (buffer_end - buffer_ptr < sizeof("0\r\n") - 1) return NEED_MORE;

        /* Check for the colon */
        if (buffer_ptr[-1] != ':') return INVALID;

        /* Handle the header value */
        result =  HandleHeaderValue(
            *header, buffer_ptr, buffer_end, readingLongHeader
        );
        if (result != VALID) return result;
    }

    /* If it is NOT a supported header */
    else {
        /* Skip the header name and the colon */
        result = SkipHeaderName(buffer_ptr, buffer_end);
        if (result != VALID) return result;

        /* Check for the remaining space */
        if (buffer_end - buffer_ptr < sizeof("0\r\n") - 1) return NEED_MORE;

        /* Skip the header value */
        result = SkipHeaderValue(buffer_ptr, buffer_end, readingLongHeader);
        if (result != VALID) return result;
    }

    /* Update the buffer and return the result */
    buffer = buffer_ptr;
    return result;
}

inline HttpResponse::ParseResult HttpResponse::ReadHttp11LongHeader(
    const char*& buffer, const char* buffer_end, bool& readingLongHeader
) noexcept { return SkipHeaderValue(buffer, buffer_end, readingLongHeader); }

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
        return true;

    switch (chr) {
        case '!': case '#': case '$': case '%': case '&': case '\'':
        case '*': case '+': case '-': case '.': case '^': case '_':
        case '`': case '|': case '~': return true;
        default: return false;
    }
}

inline constexpr HttpResponse::ParseResult HttpResponse::SkipHeaderName(
    const char*& buffer, const char* const buffer_end
) noexcept {
    /* Skip all the token chars */
    if (IsTokenChar(*buffer)) ++buffer;
    else return INVALID;
    while (buffer < buffer_end && IsTokenChar(*buffer)) ++buffer;

    /* If we've reached the end of the buffer */
    if (buffer >= buffer_end) return NEED_MORE;

    /* The next char must be a colon. If that is false, return INVALID */
    if (*buffer != ':') return INVALID;

    /* Skip the colon */
    ++buffer;

    /* Return VALID */
    return VALID;
}

inline constexpr HttpResponse::ParseResult HttpResponse::SkipHeaderValue(
    const char*& buffer, const char* const buffer_end, bool& readingLongHeader
) noexcept {
    /* Check all the header value bytes in a loop */
    while (buffer < buffer_end) {
        /* Check for \r char */
        if (buffer[0] == '\r') {
            /* If it is a last char in the buffer */
            if (buffer + 1 >= buffer_end) {
                readingLongHeader = true;
                return NEED_MORE;
            }

            /* If there is a eof (\r\n) sequence */
            if (buffer[1] == '\n') {
                /* Update the values and return the VALID value */
                buffer += 2;
                readingLongHeader = false;
                return VALID;
            }

            /* If there is not a eof seq */
            return INVALID;
        }

        /* Check for all visible english chars + obsolute text */
        if (!IsTextChar(*buffer) && !IsObsTextChar(*buffer)) return INVALID;

        /* Update the pointer */
        ++buffer;
    }

    /* If there is not the end of this header */
    readingLongHeader = true;
    return NEED_MORE;
}

inline constexpr HttpResponse::ParseResult HttpResponse::HandleHeaderValue(
    const Header header, const char*& buffer,
    const char* const buffer_end, bool& readingLongHeader
) noexcept {
    switch (header) {
        case CONNECTION:
        case CONTENT_ENCODING:
        case CONTENT_LENGTH:
        case TRAILER:
        case TRANSFER_ENCODING:
            return SkipHeaderValue(buffer, buffer_end, readingLongHeader);
    }
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
