#pragma once

#include "reflection/enum.h"
#include "type/radix_tree.h"
#include "util/class.h"
#include "http/http_transport.h"

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

    uint64_t ContentLength() const noexcept;

    void ReadHttp11Headers(HttpTransport& transport, const String& method);

private:
    enum Header : uint8_t {
        CONNECTION,
        CONTENT_ENCODING,
        CONTENT_LENGTH,
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
    ParseResult ReadHttp11ResponseLine(
        const char*& buffer, const char* buffer_end
    ) noexcept;

    [[nodiscard]]
    ParseResult ReadHttp11ResponseHeader(
        const char*& buffer, const char* buffer_end, bool& readingLongHeader
    ) noexcept;

    [[nodiscard]]
    ParseResult ReadHttp11LongHeader(
        const char*& buffer, const char* buffer_end, bool& readingLongHeader
    ) noexcept;

    [[nodiscard]]
    ParseResult HandleHeaderValue(
        Header header, const char*& buffer,
        const char* buffer_end, bool& readingLongHeader
    ) noexcept;

private:
    [[nodiscard]]
    static char LazyValidateHeader(const char*& buffer) noexcept;

    [[nodiscard]]
    static bool IsTextChar(unsigned char chr) noexcept;

    [[nodiscard]]
    static bool IsObsTextChar(unsigned char chr) noexcept;

    [[nodiscard]]
    static bool IsTokenChar(unsigned char chr) noexcept;

    [[nodiscard]]
    static ParseResult SkipHeaderName(
        const char*& buffer, const char* buffer_end
    ) noexcept;

    [[nodiscard]]
    static ParseResult SkipHeaderValue(
        const char*& buffer, const char* buffer_end, bool& readingLongHeader
    ) noexcept;

    template <uint32_t N>
    [[nodiscard]]
    static constexpr auto HeadersToLower(
        KeyValuePair<const char*, Header> (&&entries)[N],
        String (&&scratches)[N] = { }
    ) noexcept -> const KeyValuePair<const char*, Header> (&)[N];

    static const RadixTree<Header> _headers;
};

inline bool HttpResponse::IsValid() const noexcept { return _is_valid; }

inline uint16_t HttpResponse::StatusCode() const
noexcept { return _status_code; }

inline bool HttpResponse::IsChunkedEncoding() const
noexcept { return _is_chunked_encoding; }

inline uint64_t HttpResponse::ContentLength() const
noexcept { return _content_length; }

inline void HttpResponse::ReadHttp11Headers(
    HttpTransport& transport, const String& method
) {
    /* To parse the response main line */
    bool readingResponseLine = true;

    /* To read long headers below */
    bool readingLongHeader = false;

    /* Store results here */
    ParseResult result;

    do {
        /* Check for the end of the connection */
        if (transport.IsClosed()) return;

        /* Peek a new data */
        const char* buffer;
        const uint32_t peeked = transport.Peek(buffer);
        if (peeked == 0) return;
        const char* const buffer_end = buffer + peeked;

        /* Read the response line */
        if (readingResponseLine) {
            result = ReadHttp11ResponseLine(buffer, buffer_end);
            switch (result) {
                case VALID: readingResponseLine = false; break;
                case NEED_MORE: goto consume;
                case INVALID: default: return;
            }
        }

        /* While we have what to read, read it (headers) */
        do result = readingLongHeader ?
            ReadHttp11LongHeader(buffer, buffer_end, readingLongHeader) :
            ReadHttp11ResponseHeader(buffer, buffer_end, readingLongHeader);
        while (result == VALID);

        /* Consume the handled data from the buffer */
    consume:
        const uint32_t handled = peeked - (buffer_end - buffer);
        transport.Consume(handled);

        /* Check the result */
        switch (result) {
            case HEADERS_END: _is_valid = true; goto validate;
            case INVALID: return;
            default: break;
        }

        /* If we've NOT handled any bytes, and the buffer is full */
        if (handled == 0 && peeked == HttpTransport::PEEK_MAX) return;
    } while (result == NEED_MORE);

    /* Handle the status code and validate the content length */
validate:
    if (method == "HEAD"    ||
        method == "CONNECT" &&
        _status_code >= 200 && _status_code < 300 ||
        _status_code >= 100 && _status_code < 200 ||
        _status_code == 204 || _status_code == 304) {
        _content_length = 0;
        _is_chunked_encoding = false;
    }
}

inline HttpResponse::ParseResult HttpResponse::ReadHttp11ResponseLine(
    const char*& buffer, const char* const buffer_end
) noexcept {
    /* Http first bytes */
    constexpr static const char http_first_bytes[] = "HTTP/1.";

    /* Check the min response line size */
    if (buffer_end - buffer < sizeof(http_first_bytes) - 1 + 2 + 3 + 2)
        return NEED_MORE;

    /* Create a buffer pointer */
    const char* buffer_ptr = buffer;

    /* Check for the right http first bytes */
    if (!String::Equal(buffer_ptr, http_first_bytes,
                       sizeof(http_first_bytes) - 1))
        return INVALID;

    /* Skip the http first bytes */
    buffer_ptr += sizeof(http_first_bytes) - 1;

    /* Check for the http version and the after space */
    if ((buffer_ptr[0] != '0' ||
         buffer_ptr[0] != '1') &&
        buffer_ptr[1] != ' ') return INVALID;

    /* Go to the status code */
    buffer_ptr += 2;

    /* Read the response status code */
    const char* after_status_code;
    _status_code = String::ToInt<uint16_t>(
        buffer_ptr, buffer_end - buffer_ptr, after_status_code
    );
    if (after_status_code - buffer_ptr != 3 ||
        _status_code < 100 || _status_code > 599) {
        _status_code = 0;
        return INVALID;
    }

    /* Skip the response status code */
    buffer_ptr = after_status_code;

    /* Check for the right end of the status code */
    if (*buffer_ptr != ' ') {
        if (buffer_ptr[0] == '\r' && buffer_ptr[1] == '\n') {
            buffer = buffer_ptr + 2; return VALID;
        }
        else return INVALID;
    }

    /* Read and check the rest of the response line */
    while (buffer_ptr < buffer_end) {
        if (buffer_ptr[0] == '\r') {
            /* If there is an end of the buffer_ptr */
            if (buffer_end - buffer_ptr <= 1) return NEED_MORE;

            /* If there is \r\n seq, return the new pointer to the new line */
            if (buffer_ptr[1] == '\n') {
                buffer = buffer_ptr + 2; return VALID;
            }

            /* If there is only \r char, return INVALID */
            else return INVALID;
        }

        /* Check for the acceptable chars */
        if (!IsTextChar(*buffer_ptr)) return INVALID;

        /* Update the pointer */
        ++buffer_ptr;
    }

    /* If there is no line eof */
    return NEED_MORE;
}

inline HttpResponse::ParseResult HttpResponse::ReadHttp11ResponseHeader(
    const char*& buffer, const char* const buffer_end, bool& readingLongHeader
) noexcept {
    /* Check for the buffer pointer */
    if (buffer >= buffer_end) return NEED_MORE;

    /* Check for the line eof sequence (\r\n) */
    if (buffer[0] == '\r') {
        if (buffer_end - buffer <= 1) return NEED_MORE;
        if (buffer[1] != '\n') return INVALID;
        buffer += 2;
        return HEADERS_END;
    }

    /* Try to read the header */
    const Header* const header =
        _headers.Get<LazyValidateHeader>(buffer, buffer_end - buffer);

    /* Create a buffer pointer */
    const char* buffer_ptr = buffer;

    /* Create a result variable */
    ParseResult result;

    /* If it is a supported header */
    if (header != nullptr) {
        /* Set the buffer pointer after the colon char */
        buffer_ptr += get_enum_label_length(*header) + 1;

        /* Check for the buffer end */
        if (buffer_ptr >= buffer_end) return NEED_MORE;

        /* Check for the colon */
        if (buffer_ptr[-1] != ':') return INVALID;

        /* Handle the header value */
        result =  HandleHeaderValue(
            *header, buffer_ptr, buffer_end, readingLongHeader
        );

        /* Check for not valid result */
        if (result != VALID) return result;
    }

    /* If it is NOT a supported header */
    else {
        /* Skip the header name and the colon */
        result = SkipHeaderName(buffer_ptr, buffer_end);
        if (result != VALID) return result;

        /* Skip the header value */
        result = SkipHeaderValue(buffer_ptr, buffer_end, readingLongHeader);

        /* Check for invalid result */
        if (result == INVALID) return INVALID;
    }

    /* Update the buffer and return the result */
    buffer = buffer_ptr;
    return result;
}

inline HttpResponse::ParseResult HttpResponse::ReadHttp11LongHeader(
    const char*& buffer, const char* buffer_end, bool& readingLongHeader
) noexcept { return SkipHeaderValue(buffer, buffer_end, readingLongHeader); }

inline HttpResponse::ParseResult HttpResponse::HandleHeaderValue(
    const Header header, const char*& buffer,
    const char* const buffer_end, bool& readingLongHeader
) noexcept {
    /* Trim the buffer from the left */
    while (
        buffer < buffer_end && (*buffer == ' ' || *buffer == '\t')
    ) ++buffer;

    /* Check for the end of buffer */
    if (buffer >= buffer_end) return NEED_MORE;

    /* Handle the header */
    switch (header) {
        case CONNECTION: {
            constexpr static const char close_value[] = "Close";
            constexpr static const char keep_alive_value[] = "Keep-Alive";

            /* Get the buffer size */
            const uint32_t buffer_size = buffer_end - buffer;

            /* Check for the header value
             * If it starts with invalid bytes, return INVALID,
             * If it statts with right bytes
             * But it is not a full string, return NEED_MORE
             * If all is OK, go next */
            if (String::EqualIgnoreCase(
                    buffer, close_value,
                    min(sizeof(close_value) - 1, buffer_size)
                )) {
                if (buffer_size < sizeof(close_value) - 1 + 2)
                    return NEED_MORE;

                _is_connection_close = true;
                buffer += sizeof(close_value) - 1;
            } else if (String::EqualIgnoreCase(
                buffer, keep_alive_value,
                min(sizeof(keep_alive_value) - 1, buffer_size)
            )) {
                if (buffer_size < sizeof(keep_alive_value) - 1 + 2)
                    return NEED_MORE;

                _is_connection_close = false;
                buffer += sizeof(keep_alive_value) - 1;
            } else return SkipHeaderValue(
                buffer, buffer_end, readingLongHeader
            );
            break;
        }
        case CONTENT_ENCODING: {
            /* TEMPORARY UNSUPPORTED */
            return SkipHeaderValue(buffer, buffer_end, readingLongHeader);
        }
        case CONTENT_LENGTH: {
            const char* end_ptr;
            _content_length =
                String::ToInt<uint64_t>(buffer, buffer_end - buffer, end_ptr);
            if (end_ptr == buffer) return INVALID;
            buffer = end_ptr;
            break;
        }
        case TRANSFER_ENCODING: {
            constexpr static const char chunked_value[] = "Chunked";

            /* Get the buffer size */
            const uint32_t buffer_size = buffer_end - buffer;

            /* Check for the header value
             * If it starts with invalid bytes, return INVALID,
             * If it statts with right bytes
             * But it is not a full string, return NEED_MORE
             * If all is OK, go next */
            if (String::EqualIgnoreCase(
                    buffer, chunked_value,
                    min(sizeof(chunked_value) - 1, buffer_size)
                )) {
                if (buffer_size < sizeof(chunked_value) - 1 + 2)
                    return NEED_MORE;

                _is_chunked_encoding = true;
                buffer += sizeof(chunked_value) - 1;
                break;
            } else return SkipHeaderValue(
                buffer, buffer_end, readingLongHeader
            );
        }
    }

    /* Trim the buffer from the right */
    while (
        buffer < buffer_end && (*buffer == ' ' || *buffer == '\t')
    ) ++buffer;

    /* Check for the line eof (\r\n sequence) */
    if (buffer_end - buffer <= 1) return NEED_MORE;
    if (buffer[0] != '\r' || buffer[1] != '\n') return INVALID;
    buffer += 2;
    return VALID;
}

inline char HttpResponse::LazyValidateHeader(const char*& buffer)
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

inline bool HttpResponse::IsTextChar(const unsigned char chr)
noexcept { return (chr >= 0x20 && chr <= 0x7e) || chr == '\t'; }

inline bool HttpResponse::IsObsTextChar(const unsigned char chr)
noexcept { return chr >= 0x80; }

inline bool HttpResponse::IsTokenChar(const unsigned char chr)
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

inline HttpResponse::ParseResult HttpResponse::SkipHeaderName(
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

inline HttpResponse::ParseResult HttpResponse::SkipHeaderValue(
    const char*& buffer, const char* const buffer_end, bool& readingLongHeader
) noexcept {
    /* Check all the header value bytes in a loop */
    while (buffer < buffer_end) {
        /* Check for \r char */
        if (buffer[0] == '\r') {
            /* If it is a last char in the buffer */
            if (buffer_end - buffer <= 1) {
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
        { "Connection"        , CONNECTION          },
        { "Content-Encoding"  , CONTENT_ENCODING    },
        { "Content-Length"    , CONTENT_LENGTH      },
        { "Transfer-Encoding" , TRANSFER_ENCODING   },
    })
};
