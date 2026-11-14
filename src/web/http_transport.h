#pragma once

#include "type/string.h"
#include "util/class.h"

#include <cstdint>
#include <cstdio>

/**
 * A transport level for the http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpTransport final {
public:
    PREVENT_COPY_AND_MOVE(HttpTransport);

    HttpTransport(uint32_t addr, uint16_t port) noexcept;

    void Send(const char* buffer, uint32_t buffer_size) noexcept;
    void Send(const String& string) noexcept;
    template <uint32_t N>
    void Send(const char (&string)[N]) noexcept;
    template <typename T>
    void Send(T) = delete;
    void Send(char chr) noexcept;

    void Flush() noexcept;

private:
    const uint32_t _addr;
    const uint16_t _port;
};

inline HttpTransport::HttpTransport(const uint32_t addr,
                                    const uint16_t port) noexcept
    : _addr(addr), _port(port) { }

inline void HttpTransport::Send(const char* const buffer,
                                const uint32_t buffer_size)
noexcept { fwrite(buffer, 1, buffer_size, stdout); }

inline void HttpTransport::Send(const String& string)
noexcept { Send(string, string.Length()); }

template <uint32_t N>
inline void HttpTransport::Send(const char (&string)[N])
noexcept { Send(string, N - 1); }

inline void HttpTransport::Send(const char chr)
noexcept { fputc(chr, stdout); }

inline void HttpTransport::Flush() noexcept { fflush(stdout); }
