#pragma once

#include "socket/tcp_socket.h"
#include "type/string.h"
#include "util/class.h"
#include "util/minmax.h"

#include <cassert>
#include <cstdint>
#include <netinet/tcp.h>

class HttpTxEncoder;
class HttpRxEncoder;

/**
 * A transport level for the http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpTransport final {
public:
    PREVENT_COPY_AND_MOVE(HttpTransport);

    HttpTransport(HttpTxEncoder* const& tx_encoder,
                  HttpRxEncoder* const& rx_encoder,
                  uint32_t addr, uint16_t port) noexcept;

    void Connect();

    void Write(char chr);
    void Write(const char* buffer, uint32_t buffer_size);
    void Write(const String& string);
    template <uint32_t N> void Write(const char (&string)[N]);
    template <typename T> void Write(T) noexcept = delete;

    void RawWrite(char chr);
    void RawWrite(const char* buffer, uint32_t buffer_size);
    void RawWrite(const String& string);
    template <uint32_t N> void RawWrite(const char (&string)[N]);
    template <typename T> void RawWrite(T) noexcept = delete;

    void Flush();

    uint32_t Read(char* buffer, uint32_t buffer_size);

    uint32_t RawRead(char* buffer, uint32_t buffer_size);

    uint32_t RxBufferedSize() const noexcept;

    /**
     * Get a pointer to the received (but not consumed) data without consuming
     * it. The pointer is valid until the next
     * Peek()/Consume()/Read()/Reset() call.
     * @param buffer   Receives the pointer to the buffered data
     * @return Number of available bytes.
     */
    uint32_t Peek(const char*& buffer, bool use_socket = true);

    void Consume(uint32_t size);

    void Close() noexcept;

    bool IsClosed() const noexcept;

    void Reset();

private:
    HttpTxEncoder* const& _tx_encoder;
    HttpRxEncoder* const& _rx_encoder;

    const sockaddr_in _addr;
    TcpSocket _socket;

    char _tx_buffer[1460];
    const char* const _tx_buffer_end = _tx_buffer + sizeof(_tx_buffer);

    char _rx_buffer[1460];
    const char* const _rx_buffer_end = _rx_buffer + sizeof(_rx_buffer);

    char* _tx_ptr = _tx_buffer;
    char* _rx_ptr = _rx_buffer;
    char* _rx_end = _rx_buffer;

    void RawFlush();

public:
    static constexpr const uint32_t PEEK_MAX = sizeof(_rx_buffer);
};

inline HttpTransport::HttpTransport(HttpTxEncoder* const& tx_encoder,
                                    HttpRxEncoder* const& rx_encoder,
                                    const uint32_t addr,
                                    const uint16_t port)
noexcept : _tx_encoder(tx_encoder), _rx_encoder(rx_encoder), _addr({
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr { addr }
}) { }

#include "http/http_tx_encoder.h"
#include "http/http_rx_encoder.h"

inline void HttpTransport::Connect() {
    /* Set options */
    const int32_t nodelay = 1;
    _socket.SetOption(IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

    /* Connect a socket to the server */
    _socket.Connect(_addr);
}

inline void HttpTransport::Write(const char chr)
{ _tx_encoder->Write(this, chr); }

inline void HttpTransport::Write(const char* const buffer,
                                 const uint32_t buffer_size)
{ _tx_encoder->Write(this, buffer, buffer_size); }

inline void HttpTransport::Write(const String& string)
{ Write(string, string.Length()); }

template <uint32_t N> inline void HttpTransport::Write(const char (&string)[N])
{ Write(string, N - 1); }

inline void HttpTransport::RawWrite(const char chr) {
    /* Check for a free space */
    if (_tx_ptr >= _tx_buffer_end) RawFlush();

    /* Write the char */
    *_tx_ptr = chr;

    /* Update the pointer */
    ++_tx_ptr;
}

inline void HttpTransport::RawWrite(const char* buffer,
                                    uint32_t buffer_size) {
    /* While we have the data to write */
    while (buffer_size > 0) {
        /* Check for a free space */
        if (_tx_ptr >= _tx_buffer_end) RawFlush();

        /* Calculate the chunk size */
        const uint32_t chunk_size = min(buffer_size, _tx_buffer_end - _tx_ptr);

        /* Write the chunk */
        memcpy(_tx_ptr, buffer, chunk_size);

        /* Update the pointers and sizes */
        _tx_ptr += chunk_size;
        buffer      += chunk_size;
        buffer_size -= chunk_size;
    }
}

inline void HttpTransport::RawWrite(const String& string)
{ RawWrite(string.CStr(), string.Length()); }

template <uint32_t N>
inline void HttpTransport::RawWrite(const char (&string)[N])
{ RawWrite(string, N - 1); }

inline void HttpTransport::Flush() { _tx_encoder->Finish(this); RawFlush(); }

inline uint32_t HttpTransport::Read(char* const buffer,
                                    const uint32_t buffer_size)
{ return _rx_encoder->Read(this, buffer, buffer_size); }

inline uint32_t HttpTransport::RawRead(char* const buffer,
                                       const uint32_t buffer_size) {
    /* Save the total received size */
    uint32_t total_received = 0;

    /* Firstly, return the data that has been received earlier */
    if (_rx_ptr != _rx_end) {
        /* Calc the chunk size */
        const uint32_t chunk_size = min(buffer_size, _rx_end - _rx_ptr);

        /* Copy the pending data from the buffer */
        memcpy(buffer, _rx_ptr, chunk_size);
        total_received = chunk_size;

        /* Update the buffer pointer */
        _rx_ptr += chunk_size;

        /* If we have drained the buffer, rewind the pointers */
        if (_rx_ptr == _rx_end) _rx_ptr = _rx_end = _rx_buffer;
    }

    /* Receive directly to the user buffer fomr the socket */
    while (total_received < buffer_size) {
        /* Check for the socket */
        if (_socket.IsClosed()) return total_received;

        /* Get the data from the socket */
        const uint32_t currently_received =
            _socket.Receive(buffer + total_received,
                            buffer_size - total_received);

        /* If the connection has been closed,
         * Return the result and exit */
        if (currently_received == 0) {
            _socket.Close(); return total_received;
        }

        /* Update the total received size */
        total_received += currently_received;
    }

    /* Return the result */
    return total_received;
}

inline uint32_t HttpTransport::RxBufferedSize() const
noexcept { return _rx_end - _rx_ptr; }

inline uint32_t HttpTransport::Peek(
    const char*& buffer, const bool use_socket
) {
    /* Get the available data size */
    const uint32_t available = RxBufferedSize();

    /* The buffer is empty: just rewind the pointers
     * to make the whole buffer free again */
    if (available == 0) _rx_ptr = _rx_end = _rx_buffer;

    /* Move the pending data to the beginning of the buffer
     * If there is less than 16 bytes available at the end */
    else if (_rx_ptr > _rx_buffer &&
             _rx_buffer_end - _rx_end <= 16) {
        memmove(_rx_buffer, _rx_ptr, available);
        _rx_ptr = _rx_buffer;
        _rx_end = _rx_buffer + available;
    }

    /* Set the buffer pointer to the rx pointer */
    buffer = _rx_ptr;

    /* If we can't use the socket */
    /* If we have not enought space */
    /* If the connection is closed */
    if (!use_socket ||
        _rx_end >= _rx_buffer_end ||
        _socket.IsClosed()) return available;

    /* Receive the data from the socket */
    const uint32_t received = _socket.Receive(
        _rx_end, _rx_buffer_end - _rx_end
    );

    /* If the connection has been closed */
    if (received == 0) _socket.Close();

    /* Update the rx end pointer */
    else _rx_end += received;

    /* Return the result */
    return available + received;
}

inline void HttpTransport::Consume(const uint32_t size) {
    assert(size <= _rx_end - _rx_ptr);

    /* Increase the pointer */
    _rx_ptr += size;

    /* If there is no pending data, rewind the pointers
     * to make the whole buffer free again */
    if (_rx_ptr == _rx_end) _rx_ptr = _rx_end = _rx_buffer;
}

inline void HttpTransport::Close() noexcept { _socket.Close(); }

inline bool HttpTransport::IsClosed() const
noexcept { return _socket.IsClosed(); }

inline void HttpTransport::Reset() {
    /* Open a new TCP socket */
    _socket = TcpSocket();

    /* Reset the buffer pointers */
    _tx_ptr = _tx_buffer;
    _rx_ptr = _rx_end = _rx_buffer;
}

inline void HttpTransport::RawFlush() {
    /* Check for an empty buffer */
    if (_tx_ptr == _tx_buffer) return;

    /* Check for the socket state */
    assert(!_socket.IsClosed());

    /* Write the data to the socket */
    _socket.Send(_tx_buffer, _tx_ptr - _tx_buffer);

    /* Reset the buffer pointer */
    _tx_ptr = _tx_buffer;
}
