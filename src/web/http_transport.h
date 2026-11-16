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
    class Encoder {
    public:
        ALLOW_COPY_ALLOW_MOVE(Encoder);
        virtual ~Encoder() = default;

        virtual void Write(HttpTransport* transport, char chr) const  = 0;
        virtual void Write(HttpTransport* transport,
                           const char* buffer,
                           uint32_t buffer_size) const = 0;

        virtual void Finish(HttpTransport* transport) const = 0;

        class Plain;
        class Chunked;

    protected:
        Encoder() = default;
    };

    PREVENT_COPY_AND_MOVE(HttpTransport);

    HttpTransport(uint32_t addr, uint16_t port) noexcept;

    void SetTxEncoder(const Encoder& encoder) noexcept;

    void SetRxEncoder(const Encoder& encoder) noexcept;

    void Connect();

    void Write(char chr);
    void Write(const char* buffer, uint32_t buffer_size);
    void Write(const String& string);
    template <uint32_t N> void Write(const char (&string)[N]);
    template <typename T> void Write(T) noexcept = delete;

    void Flush();

    uint32_t Read(char* buffer, uint32_t buffer_size);

    void Abort();

private:
    const Encoder* _tx_encoder;
    const Encoder* _rx_encoder;

    const sockaddr_in _addr;
    TcpSocket _socket;

    char _buffer[1460];
    char* _buffer_ptr = _buffer;
    const char* const _buffer_end = _buffer + sizeof(_buffer);

    void RawWrite(char chr);
    void RawWrite(const char* buffer, uint32_t buffer_size);
    void RawWrite(const String& string);
    template <uint32_t N> void RawWrite(const char (&string)[N]);
    template <typename T> void RawWrite(T) noexcept = delete;

    void RawFlush();
};

using HttpEncoder = HttpTransport::Encoder;

class HttpEncoder::Plain final : public Encoder {
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

class HttpEncoder::Chunked final : public Encoder {
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

inline void HttpEncoder::Plain::Write(HttpTransport* const transport,
                                        char chr)
const { transport->RawWrite(chr); }

inline void HttpEncoder::Plain::Write(HttpTransport* const transport,
                                      const char* const buffer,
                                      const uint32_t buffer_size) const
{ transport->RawWrite(buffer, buffer_size); }

inline void HttpEncoder::Plain::Finish(HttpTransport* const transport) const
noexcept { }

inline HttpEncoder::Plain& HttpEncoder::Plain::Instance() noexcept {
    static Plain instance;
    return instance;
}

inline void HttpEncoder::Chunked::Write(HttpTransport* const transport,
                                        char chr) const {
    transport->RawWrite("1\r\n");
    transport->RawWrite(chr);
    transport->RawWrite("\r\n");
}

inline void HttpEncoder::Chunked::Write(HttpTransport* const transport,
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

inline void
HttpEncoder::Chunked::Finish(HttpTransport* const transport)
const { transport->RawWrite("0\r\n\r\n");  }

inline HttpEncoder::Chunked& HttpEncoder::Chunked::Instance() noexcept {
    static Chunked instance;
    return instance;
}

inline HttpTransport::HttpTransport(const uint32_t addr,
                                    const uint16_t port) noexcept : _addr({
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr { addr }
}) { }

inline void HttpTransport::SetTxEncoder(const Encoder& encoder)
noexcept { _tx_encoder = &encoder; }

inline void HttpTransport::SetRxEncoder(const Encoder& encoder)
noexcept { _rx_encoder = &encoder; }

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

inline void HttpTransport::Flush() { _tx_encoder->Finish(this); RawFlush(); }

inline uint32_t HttpTransport::Read(char* const buffer,
                                    const uint32_t buffer_size)
{ return _socket.Receive(buffer, buffer_size); }

inline void HttpTransport::Abort() {
    /* Open a new TCP socket */
    _socket = TcpSocket();

    /* Reset the buffer pointer */
    _buffer_ptr = _buffer;

    /* Reset the encoders */
    _tx_encoder = nullptr;
    _rx_encoder = nullptr;
}

inline void HttpTransport::RawWrite(const char chr) {
    /* Check for a free space */
    if (_buffer_ptr >= _buffer_end) RawFlush();

    /* Write the char */
    *_buffer_ptr = chr;

    /* Update the pointer */
    ++_buffer_ptr;
}

inline void HttpTransport::RawWrite(const char* buffer,
                                    uint32_t buffer_size) {
    /* While we have the data to write */
    while (buffer_size > 0) {
        /* Check for a free space */
        if (_buffer_ptr >= _buffer_end) RawFlush();

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

inline void HttpTransport::RawWrite(const String& string)
{ RawWrite(string.CStr(), string.Length()); }

template <uint32_t N>
inline void HttpTransport::RawWrite(const char (&string)[N])
{ RawWrite(string, N - 1); }

inline void HttpTransport::RawFlush() {
    /* Check for an empty buffer */
    if (_buffer_ptr == _buffer) return;

    /* Write the data to the socket */
    _socket.Send(_buffer, _buffer_ptr - _buffer);
    fwrite(_buffer, 1, _buffer_ptr - _buffer, stdout);
    fflush(stdout);

    /* Reset the buffer pointer */
    _buffer_ptr = _buffer;
}
