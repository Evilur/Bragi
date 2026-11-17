#pragma once

#include "type/uniq_ptr.h"
#include "util/class.h"
#include "web/http_transport.h"

#include <cstdint>

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

    bool BodyRemains() const noexcept;

    void ReadHttp11Headers(HttpTransport& transport) noexcept;

    void WriteRemainingBody(const char* buffer, uint32_t buffer_size) noexcept;

private:
    bool _is_valid = false;
    uint16_t _status_code = 0;
    bool _is_chunked_encoding = false;

    UniqPtr<char[]> _remaining_body = nullptr;
    uint32_t _remaining_body_size = 0;
};

inline bool HttpResponse::IsValid() const noexcept { return _is_valid; }

inline uint16_t HttpResponse::StatusCode() const
noexcept { return _status_code; }

inline bool HttpResponse::IsChunkedEncoding() const
noexcept { return _is_chunked_encoding; }

inline bool HttpResponse::BodyRemains() const
noexcept { return _remaining_body_size > 0; }

inline void HttpResponse::ReadHttp11Headers(HttpTransport& transport)
noexcept {

}

inline void HttpResponse::WriteRemainingBody(const char* const buffer,
                                             const uint32_t buffer_size)
noexcept {

}
