#pragma once

#include "socket/tcp_socket.h"
#include "type/string.h"
#include "util/class.h"
#include "util/minmax.h"

#include <cassert>
#include <cstdint>
#include <netinet/tcp.h>

/**
 * A transport level for the http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpTransport final {
public:
    class TxEncoder {
    public:
        ALLOW_COPY_ALLOW_MOVE(TxEncoder);
        virtual ~TxEncoder() = default;

        virtual void Write(HttpTransport* transport, char chr) const = 0;
        virtual void Write(HttpTransport* transport,
                           const char* buffer,
                           uint32_t buffer_size) const = 0;

        virtual void Finish(HttpTransport* transport) const = 0;

        class Plain;
        class Chunked;

    protected:
        TxEncoder() = default;
    };

    class RxEncoder {
    public:
        ALLOW_COPY_ALLOW_MOVE(RxEncoder);
        virtual ~RxEncoder() = default;

        virtual uint32_t Read(HttpTransport* transport,
                              char* buffer,
                              uint32_t buffer_size) = 0;

        class Plain;
        class Chunked;

    protected:
        RxEncoder() = default;
    };

    PREVENT_COPY_AND_MOVE(HttpTransport);

    HttpTransport(TxEncoder* const& tx_encoder,
                  RxEncoder* const& rx_encoder,
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

    /**
     * Get a pointer to the received (but not consumed) data without consuming
     * it. If less than `min_size` bytes are buffered, reads more from the
     * socket. The pointer is valid until the next
     * Peek()/Consume()/Read()/Abort() call.
     * @param buffer   Receives the pointer to the buffered data
     * @param min_size Min amount of bytes to wait for (<= buffer capacity).
     *                 0 means: don't touch the socket,
     *                 just return what we have
     * @return Number of available bytes. It is less than `min_size`
     *         only if the connection has been closed (0 = EOF)
     */
    uint32_t Peek(const char*& buffer, uint32_t min_size = 0);

    void Consume(uint32_t size);

    void Abort();

private:
    TxEncoder* const& _tx_encoder;
    RxEncoder* const& _rx_encoder;

    const sockaddr_in _addr;
    TcpSocket _socket;

    char _tx_buffer[1460];
    char* _tx_ptr = _tx_buffer;
    const char* const _tx_end = _tx_buffer + sizeof(_tx_buffer);

    char _rx_buffer[1460];
    char* _rx_ptr = _rx_buffer;
    char* _rx_end = _rx_buffer;

    void RawFlush();

public:
    static constexpr const uint32_t PEEK_MAX = sizeof(_rx_buffer);
};

using HttpTxEncoder = HttpTransport::TxEncoder;
using HttpRxEncoder = HttpTransport::RxEncoder;

class HttpTxEncoder::Plain final : public TxEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Plain);

    void Write(HttpTransport* transport, char chr) const override;
    void Write(HttpTransport* transport,
               const char* buffer,
               uint32_t buffer_size) const override;

    void Finish(HttpTransport* transport) const noexcept override;

private:
    Plain() = default;

public:
    static Plain& Instance() noexcept;
};

class HttpTxEncoder::Chunked final : public TxEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Chunked);

    void Write(HttpTransport* transport, char chr) const override;
    void Write(HttpTransport* transport,
               const char* buffer,
               uint32_t buffer_size) const override;

    void Finish(HttpTransport* transport) const override;

private:
    Chunked() = default;

public:
    static Chunked& Instance() noexcept;
};

class HttpRxEncoder::Plain final : public RxEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Plain);

    uint32_t Read(HttpTransport* transport,
                  char* buffer,
                  uint32_t buffer_size) override;

private:
    Plain() = default;

public:
    static Plain& Instance() noexcept;
};

class HttpRxEncoder::Chunked final : public RxEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Chunked);
    Chunked() = default;

    uint32_t Read(HttpTransport* transport,
                  char* buffer,
                  uint32_t buffer_size) override;

    void Reset() noexcept;

private:
    uint32_t _current_chunk_size = 0;
};

inline void HttpTxEncoder::Plain::Write(HttpTransport* const transport,
                                        char chr)
const { transport->RawWrite(chr); }

inline void HttpTxEncoder::Plain::Write(HttpTransport* const transport,
                                      const char* const buffer,
                                      const uint32_t buffer_size)
const { transport->RawWrite(buffer, buffer_size); }

inline void HttpTxEncoder::Plain::Finish(HttpTransport* const transport)
const noexcept { }

inline HttpTxEncoder::Plain& HttpTxEncoder::Plain::Instance() noexcept {
    static Plain instance;
    return instance;
}

inline void HttpTxEncoder::Chunked::Write(HttpTransport* const transport,
                                        char chr) const {
    transport->RawWrite("1\r\n");
    transport->RawWrite(chr);
    transport->RawWrite("\r\n");
}

inline void HttpTxEncoder::Chunked::Write(HttpTransport* const transport,
                                        const char* const buffer,
                                        const uint32_t buffer_size) const {
    /* Check for an empty chunk */
    if (buffer_size == 0) return;

    /* Create a buffer for the chunk size string representation */
    constexpr char hex_alpha[] = "0123456789abcdef";
    char chunk_size[sizeof(uint32_t) * 2];
    char* chunk_size_ptr = chunk_size + sizeof(chunk_size);

    /* Calculate the chunk size */
    {
        uint32_t value = buffer_size;
        do {
            --chunk_size_ptr;
            *chunk_size_ptr = hex_alpha[value & 0xF];
            value >>= 4;
        } while (value > 0);
    }

    /* Send the chunk size */
    transport->RawWrite(chunk_size_ptr,
                        sizeof(chunk_size) - (chunk_size_ptr - chunk_size));
    transport->RawWrite("\r\n");

    /* Send the body */
    transport->RawWrite(buffer, buffer_size);
    transport->RawWrite("\r\n");
}

inline void HttpTxEncoder::Chunked::Finish(HttpTransport* const transport)
const { transport->RawWrite("0\r\n\r\n"); }

inline HttpTxEncoder::Chunked& HttpTxEncoder::Chunked::Instance() noexcept {
    static Chunked instance;
    return instance;
}

inline uint32_t HttpRxEncoder::Plain::Read(HttpTransport* const transport,
                                           char* const buffer,
                                           const uint32_t buffer_size)
{ return transport->RawRead(buffer, buffer_size); }

inline HttpRxEncoder::Plain& HttpRxEncoder::Plain::Instance() noexcept {
    static Plain instance;
    return instance;
}

inline uint32_t HttpRxEncoder::Chunked::Read(HttpTransport* const transport,
                                             char* const buffer,
                                             const uint32_t buffer_size) {
    return 0;
}

inline void HttpRxEncoder::Chunked::Reset()
noexcept { _current_chunk_size = 0; }

inline HttpTransport::HttpTransport(TxEncoder* const& tx_encoder,
                                    RxEncoder* const& rx_encoder,
                                    const uint32_t addr,
                                    const uint16_t port)
noexcept : _tx_encoder(tx_encoder), _rx_encoder(rx_encoder), _addr({
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr { addr }
}) { }

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
    if (_tx_ptr >= _tx_end) RawFlush();

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
        if (_tx_ptr >= _tx_end) RawFlush();

        /* Calculate the chunk size */
        const uint32_t chunk_size = min(buffer_size, _tx_end - _tx_ptr);

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
        /* Get the data from the socket */
        const uint32_t currently_received =
            _socket.Receive(buffer + total_received,
                            buffer_size - total_received);

        /* If the connection has been closed,
         * Return the result and exit */
        if (currently_received == 0) return total_received;

        /* Update the total received size */
        total_received += currently_received;
    }

    /* Return the result */
    return total_received;
}

inline uint32_t HttpTransport::Peek(const char*& buffer,
                                    const uint32_t min_size) {
    assert(min_size <= sizeof(_rx_buffer));

    /* Get the available data size */
    uint32_t available = _rx_end - _rx_ptr;

    /* If we have not enough data, get the rest */
    if (available < min_size) {
        const char* const rx_limit = _rx_buffer + sizeof(_rx_buffer);

        /* The buffer is empty: just rewind the pointers
         * to make the whole buffer free again */
        if (available == 0)
            _rx_ptr = _rx_end = _rx_buffer;

        /* Not enough room after the read pointer:
         * move the pending data to the beginning of the buffer */
        else if (rx_limit - _rx_ptr < min_size) {
            memmove(_rx_buffer, _rx_ptr, available);
            _rx_ptr = _rx_buffer;
            _rx_end = _rx_buffer + available;
        }

        /* Receive until we have enough data.
         * Here we always have free space after _rx_end,
         * because available < min_size <= rx_limit - _rx_ptr */
        do {
            /* Receive the data from the socket */
            const uint32_t received =
                _socket.Receive(_rx_end, rx_limit - _rx_end);

            /* The connection has been closed */
            if (received == 0) break;

            /* Update the pointer and the size */
            _rx_end   += received;
            available += received;
        } while (available < min_size);
    }

    /* Return the result */
    buffer = _rx_ptr;
    return available;
}

inline void HttpTransport::Consume(const uint32_t size) {
    assert(size <= _rx_end - _rx_ptr);

    /* Increase the pointer */
    _rx_ptr += size;

    /* If there is no pending data, rewind the pointers
     * to make the whole buffer free again */
    if (_rx_ptr == _rx_end) _rx_ptr = _rx_end = _rx_buffer;
}

inline void HttpTransport::Abort() {
    /* Open a new TCP socket */
    _socket = TcpSocket();

    /* Reset the buffer pointers */
    _tx_ptr = _tx_buffer;
    _rx_ptr = _rx_end = _rx_buffer;
}

inline void HttpTransport::RawFlush() {
    /* Check for an empty buffer */
    if (_tx_ptr == _tx_buffer) return;

    /* Write the data to the socket */
    _socket.Send(_tx_buffer, _tx_ptr - _tx_buffer);

    /* Reset the buffer pointer */
    _tx_ptr = _tx_buffer;
}
