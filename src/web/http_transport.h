#pragma once

#include "socket/tcp_socket.h"
#include "type/string.h"
#include "util/class.h"

#include <cstdint>
#include <netinet/tcp.h>

/**
 * A transport level for the http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpTransport final {
public:
    PREVENT_COPY_AND_MOVE(HttpTransport);

    HttpTransport(uint32_t addr, uint16_t port);

    void Send(char chr) noexcept;
    void Send(const char* buffer, uint32_t buffer_size) noexcept;
    void Send(const String& string) noexcept;
    template <uint32_t N> void Send(const char (&string)[N]) noexcept;
    template <typename T> void Send(T) = delete;

    void Flush() noexcept;

    uint32_t Receive(char* buffer, uint32_t buffer_size) noexcept;

private:
    TcpSocket _socket;

    char _buffer[1460];
    char* _buffer_ptr = _buffer;
    const char* const _buffer_end = _buffer + sizeof(_buffer);
};

inline HttpTransport::HttpTransport(const uint32_t addr,
                                    const uint16_t port) {
    /* Set options */
    int32_t nodelay = 1;
    _socket.SetOption(IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

    /* Connect a socket to the server */
    _socket.Connect({
        .sin_family = AF_INET,
        .sin_port = port,
        .sin_addr { addr }
    });
}

inline void HttpTransport::Send(const char chr) noexcept {
    /* Check for a free space */
    if (_buffer_ptr >= _buffer_end) Flush();

    /* Write the char */
    *_buffer_ptr = chr;

    /* Update the pointer */
    ++_buffer_ptr;
}

inline void HttpTransport::Send(const char* buffer,
                                uint32_t buffer_size) noexcept {
    /* While we have the data to write */
    while (buffer_size > 0) {
        /* Check for a free space */
        if (_buffer_ptr >= _buffer_end) Flush();

        /* Calculate the chunk size */
        const uint32_t free_space = _buffer_end - _buffer_ptr;
        const uint32_t chunk_size = buffer_size < free_space ?
                                    buffer_size : free_space;

        /* Write the chunk */
        memcpy(_buffer_ptr, buffer, chunk_size);

        /* Update the pointers and sizes */
        _buffer_ptr += chunk_size;
        buffer      += chunk_size;
        buffer_size -= chunk_size;
    }
}

inline void HttpTransport::Send(const String& string)
noexcept { Send(string, string.Length()); }

template <uint32_t N>
inline void HttpTransport::Send(const char (&string)[N])
noexcept { Send(string, N - 1); }

inline void HttpTransport::Flush() noexcept {
    /* Check for an empty buffer */
    if (_buffer_ptr == _buffer) return;

    /* Write the data to the socket */
    if (!_socket.Send(_buffer, _buffer_ptr - _buffer)) {
        ERROR_LOG("Failed to flush the http buffer");
        return;
    }

    /* Reset the buffer pointer */
    _buffer_ptr = _buffer;
}

inline uint32_t HttpTransport::Receive(char* const buffer,
                                       const uint32_t buffer_size)
noexcept { return _socket.Receive(buffer, buffer_size); }
