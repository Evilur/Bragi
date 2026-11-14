#pragma once

#include "type/string.h"
#include "type/uniq_ptr.h"
#include "util/class.h"

#include <cstdint>
#include <cstdio>

/**
 * A transport level for the http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpTransport final {
public:
    enum Encoding : uint8_t { PLAIN_BODY, CHUNKED };

    struct Encoder {
        PREVENT_COPY_AND_MOVE(Encoder);
        Encoder() = default;
        virtual ~Encoder() = default;

        virtual void Send(HttpTransport& transport, char chr) noexcept = 0;
        virtual void Send(HttpTransport& transport,
                          const char* buffer,
                          uint32_t buffer_size) noexcept = 0;

        virtual void Finish(HttpTransport& transport) noexcept = 0;
    };

    struct PlainBody final : public Encoder {
        PREVENT_COPY_AND_MOVE(PlainBody);
        PlainBody() = default;

        void Send(HttpTransport& transport, char chr) noexcept override;
        void Send(HttpTransport& transport,
                  const char* buffer,
                  uint32_t buffer_size) noexcept override;

        void Finish(HttpTransport& transport) noexcept override;
    };

    struct Chunked final : public Encoder {
        PREVENT_COPY_AND_MOVE(Chunked);
        Chunked() = default;

        void Send(HttpTransport& transport, char chr) noexcept override;
        void Send(HttpTransport& transport,
                  const char* buffer,
                  uint32_t buffer_size) noexcept override;

        void Finish(HttpTransport& transport) noexcept override;
    };

    PREVENT_COPY_AND_MOVE(HttpTransport);

    HttpTransport(uint32_t addr, uint16_t port, Encoding encoding);

    void SetEncoding(Encoding encoding);

    void Send(char chr) noexcept;
    void Send(const char* buffer, uint32_t buffer_size) noexcept;
    void Send(const String& string) noexcept;
    template <uint32_t N> void Send(const char (&string)[N]) noexcept;
    template <typename T> void Send(T) = delete;

    void Finish() noexcept;

private:
    UniqPtr<Encoder> _encoder;

    char _buffer[1460];
    char* _buffer_ptr = _buffer;
    const char* const _buffer_end = _buffer + sizeof(_buffer);

    void Write(char chr) noexcept;
    void Write(const char* buffer, uint32_t buffer_size) noexcept;
    template <uint32_t N> void Write(const char (&string)[N]) noexcept;
    template <typename T> void Write(T) = delete;

    void Flush() noexcept;

private:
    static Encoder* GetEncoder(Encoding encoding);
};

inline void HttpTransport::PlainBody::Send(HttpTransport& transport,
                                           const char chr)
noexcept { transport.Write(chr); }

inline void HttpTransport::PlainBody::Send(HttpTransport& transport,
                                           const char* const buffer,
                                           const uint32_t buffer_size)
noexcept { transport.Write(buffer, buffer_size); }

inline void HttpTransport::PlainBody::Finish(HttpTransport& transport)
noexcept { }

inline void HttpTransport::Chunked::Send(HttpTransport& transport,
                                         const char chr) noexcept {
    transport.Write("1\r\n");
    transport.Write(chr);
    transport.Write("\r\n");
}

inline void HttpTransport::Chunked::Send(HttpTransport& transport,
                                         const char* const buffer,
                                         const uint32_t buffer_size) noexcept {
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
    transport.Write(chunk_size_ptr,
                    sizeof(chunk_size) - (chunk_size_ptr - chunk_size));
    transport.Write("\r\n");

    /* Send the body */
    transport.Write(buffer, buffer_size);
    transport.Write("\r\n");
}

inline void
HttpTransport::Chunked::Finish(HttpTransport& transport)
noexcept { transport.Write("0\r\n\r\n");  }

inline HttpTransport::HttpTransport(const uint32_t addr,
                                    const uint16_t port,
                                    const Encoding encoding)
: _encoder(GetEncoder(encoding)) { }

inline void HttpTransport::SetEncoding(const Encoding encoding)
{ _encoder = GetEncoder(encoding); }

inline void HttpTransport::Send(const char chr)
noexcept { _encoder->Send(*this, chr); }

inline void HttpTransport::Send(const char* const buffer,
                                const uint32_t buffer_size)
noexcept { _encoder->Send(*this, buffer, buffer_size); }

inline void HttpTransport::Send(const String& string)
noexcept { Send(string, string.Length()); }

template <uint32_t N>
inline void HttpTransport::Send(const char (&string)[N])
noexcept { Send(string, N - 1); }

inline void HttpTransport::Finish()
noexcept { _encoder->Finish(*this); Flush(); }

inline void HttpTransport::Write(const char chr) noexcept {
    /* Check for a free space */
    if (_buffer_ptr >= _buffer_end) Flush();

    /* Write the char */
    *_buffer_ptr = chr;

    /* Update the pointer */
    ++_buffer_ptr;
}

inline void HttpTransport::Write(const char* buffer,
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

template <uint32_t N>
inline void HttpTransport::Write(const char (&string)[N])
noexcept { Write(string, N - 1); }

inline void HttpTransport::Flush() noexcept {
    /* Check for an empty buffer */
    if (_buffer_ptr == _buffer) return;

    fwrite(_buffer, 1, _buffer_ptr - _buffer, stdout);
    fflush(stdout);

    /* Reset the buffer pointer */
    _buffer_ptr = _buffer;
}

inline HttpTransport::Encoder*
HttpTransport::GetEncoder(const Encoding encoding) {
    switch (encoding) {
        case PLAIN_BODY: return new PlainBody();
        case CHUNKED: return new Chunked();
    }
    return nullptr;
}
