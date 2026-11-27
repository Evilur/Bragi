#pragma once

#include "type/nullable.h"
#include "util/class.h"
#include "util/minmax.h"
#include "type/string.h"
#include "http/http_transport.h"

#include <cstdint>

class HttpRxEncoder {
public:
    ALLOW_COPY_ALLOW_MOVE(HttpRxEncoder);
    virtual ~HttpRxEncoder() = default;

    virtual uint32_t Read(HttpTransport* transport,
                          char* buffer,
                          uint32_t buffer_size) = 0;

    class Plain;
    class Chunked;

protected:
    HttpRxEncoder() = default;
};

class HttpRxEncoder::Plain final : public HttpRxEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Plain);

    Plain() noexcept = default;
    Plain(uint64_t content_length) noexcept;

    uint32_t Read(HttpTransport* transport,
                  char* buffer,
                  uint32_t buffer_size) override;

private:
    Nullable<uint64_t> _content_length = nullptr;
};

class HttpRxEncoder::Chunked final : public HttpRxEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Chunked);
    Chunked() = default;

    uint32_t Read(HttpTransport* transport,
                  char* buffer,
                  uint32_t buffer_size) override;

    void Reset() noexcept;

private:
    uint64_t _current_chunk_size = 0;
    bool _end = false;
};

inline HttpRxEncoder::Plain::Plain(const uint64_t content_length)
noexcept : _content_length(content_length) { }

inline uint32_t HttpRxEncoder::Plain::Read(HttpTransport* const transport,
                                           char* const buffer,
                                           const uint32_t buffer_size) {
    if (_content_length == nullptr)
        return transport->RawRead(buffer, buffer_size);
    else if (*_content_length == 0) return 0;
    else {
        const uint32_t received =
            transport->RawRead(buffer, min(buffer_size, *_content_length));
        *_content_length -= received;
        return received;
    }
}

inline uint32_t HttpRxEncoder::Chunked::Read(HttpTransport* const transport,
                                             char* buffer,
                                             const uint32_t buffer_size) {
    /* Check for the end */
    if (_end) return 0;

    /* Save total bytes received here */
    uint32_t total_received = 0;

    /* Save the end of the buffer */
    const char* const buffer_end = buffer + buffer_size;

    /* Read chunk by chunk */
    while (total_received < buffer_size) {
        /* Read the chunk size */
        for (bool use_socket = false;
             _current_chunk_size == 0;
             use_socket = true) {
            /* Get the rx buffer data size */
            const uint32_t rx_buffer_size = transport->RxBufferedSize();

            /* If the transport is closed
             * And the buffer has not enough data */
            if (transport->IsClosed() && rx_buffer_size < sizeof("0\r\n") - 1)
                return total_received;

            /* Peek the chunk size */
            const char* chunk_size_str;
            const uint32_t peeked =
                transport->Peek(chunk_size_str, use_socket);
            if (peeked < sizeof("0\r\n") - 1) {
                if (use_socket && peeked == rx_buffer_size)
                    return total_received;
                else continue;
            }

            /* Get the current chunk size */
            const char* after_chunk_size;
            const uint64_t current_chunk_size = String::HexToInt<uint64_t>(
                chunk_size_str,
                min(peeked, sizeof(uint64_t) * 2),
                after_chunk_size
            );

            /* Check for the empty size value */
            if (after_chunk_size == chunk_size_str) {
                transport->Close();
                return total_received;
            }

            /* If we have not enough data to read the \r\n seq */
            if (after_chunk_size + 1 - chunk_size_str >= peeked) {
                if (transport->IsClosed()) return total_received;
                continue;
            }

            /* Check for the \r\n seq */
            if (after_chunk_size[0] != '\r' ||
                after_chunk_size[1] != '\n') {
                transport->Close();
                return total_received;
            }
            after_chunk_size += 2;

            /* Check for the end (zero) chunk */
            if (current_chunk_size == 0) {
                /* Check the buffer size */
                if (after_chunk_size + 1 - chunk_size_str >= peeked) {
                    if (transport->IsClosed()) return total_received;
                    continue;
                }

                /* Check for the \r\n seq */
                if (after_chunk_size[0] != '\r' ||
                    after_chunk_size[1] != '\n') {
                    transport->Close();
                    return total_received;
                }
                after_chunk_size += 2;
                _end = true;
            }

            /* Consume the peeked buffer */
            transport->Consume(after_chunk_size - chunk_size_str);
            _current_chunk_size = current_chunk_size;
            if (_end) return total_received;
        }

        /* Read the chunk */
        const uint32_t received = transport->RawRead(
            buffer, min(buffer_end - buffer, _current_chunk_size)
        );

        /* If the connection has been closed */
        if (received == 0) { return total_received; }

        /* Update the values */
        _current_chunk_size -= received;
        total_received      += received;
        buffer              += received;

        /* Check if we reach the end of the chunk */
        if (_current_chunk_size == 0) {
            for (bool use_socket = false;; use_socket = true) {
                /* Get the rx buffer data size */
                const uint32_t rx_buffer_size = transport->RxBufferedSize();

                /* If the transport is closed
                 * And the buffer has not enough data */
                if (transport->IsClosed() &&
                    rx_buffer_size < sizeof("\r\n") - 1)
                    return total_received;

                /* Peek the \r\n sequence after the chunk */
                const char* end_seq;
                const uint32_t peeked = transport->Peek(end_seq, use_socket);
                if (peeked < sizeof("\r\n") - 1) {
                    if (use_socket && peeked == rx_buffer_size)
                        return total_received;
                    else continue;
                }

                /* Check for the \r\n sequence */
                if (end_seq[0] != '\r' ||
                    end_seq[1] != '\n') {
                    transport->Close(); return total_received;
                }

                /* Consume the peeked buffer */
                transport->Consume(2);
                break;
            }
        }
    }

    /* Return the total received bytes number */
    return total_received;
}

inline void HttpRxEncoder::Chunked::Reset()
noexcept { _current_chunk_size = 0; _end = false; }
