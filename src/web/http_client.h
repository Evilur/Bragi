#pragma once

#include "util/class.h"
#include "web/dns.h"
#include "web/http_encoder.h"
#include "web/http_request.h"
#include "web/http_transport.h"

#include <cstdint>

/**
 * A simple http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpClient {
public:
    PREVENT_COPY_AND_MOVE(HttpClient);

    HttpClient(const char* domain,
               const char* path,
               const char* method = "GET") noexcept;

    HttpClient(const HttpRequest& request) noexcept;

    virtual ~HttpClient() noexcept = default;

    HttpRequest& Request() noexcept;

    virtual bool Write(const char* buffer, uint32_t buffer_size);

    bool Send();

    virtual uint32_t Read(char* buffer, uint32_t buffer_size);

protected:
    virtual uint16_t Port() const noexcept;

private:
    HttpRequest _request;
    HttpTransport _transport;
    HttpEncoder& _encoder = HttpEncoder::Plain::Instance();

    enum State : uint8_t {
        SENDING_HEADERS,
        SENDING_BODY,
        RECVING_HEADERS,
        RECVING_BODY
    } _state = SENDING_HEADERS;

    void Abort();
};

inline HttpClient::HttpClient(const char* const domain,
                              const char* const path,
                              const char* const method) noexcept
: _request(domain, path, method), _transport(DNS::Resolve(domain), Port()) { }

inline HttpClient::HttpClient(const HttpRequest& request) noexcept
: _request(request), _transport(DNS::Resolve(request.domain), Port()) { }

inline HttpRequest& HttpClient::Request() noexcept { return _request; }

inline bool HttpClient::Write(const char* const buffer,
                              const uint32_t buffer_size) {
    /* If we haven't sent the headers yet, do it now */
    if (_state == SENDING_HEADERS) {
        /* Connect the transport to the server */
        try { _transport.Connect(); }
        catch (const SocketException& e)
        { throw HttpException("Failed to connect to the server"); }

        /* Send the request headers */
        try { _request.SendHttp11Headers(_transport); }
        catch (const SocketException& e)
        { Abort(); return false; }

        /* Set the right encoding */
        if (_request.IsChunkedEncoding())
            _encoder = HttpEncoder::Chunked::Instance();
        else
            _encoder = HttpEncoder::Plain::Instance();

        /* Else reset the Content-Length */
        _request.ResetContentLength();

        /* Update the state */
        _state = SENDING_BODY;
    }

    /* Send the body */
    try { _encoder.Send(_transport, buffer, buffer_size); }
    catch (const SocketException& e)
    { Abort(); return false; }

    /* Return the success code */
    return true;
}

inline bool HttpClient::Send() {
    /* If we haven't sent the headers yet (not initialized transport)
     * Do it now */
    if (_state == SENDING_HEADERS) {
        /* Connect the transport to the server */
        try { _transport.Connect(); }
        catch (const SocketException& e)
        { throw HttpException("Failed to connect to the server"); }

        /* Set the 'Content-Length' to 0 and send the request headers */
        _request.SetContentLength(0);
        try { _request.SendHttp11Headers(_transport); }
        catch (const SocketException& e)
        { Abort(); return false; }

        /* Reset the Content-Length */
        _request.ResetContentLength();

        /* Set the right encoder */
        _encoder = HttpEncoder::Plain::Instance();
    }

    /* Finish the request */
    try { _encoder.Finish(_transport); }
    catch (const SocketException& e)
    { Abort(); return false; }

    /* Update the state */
    _state = RECVING_HEADERS;

    /* Return the success code */
    return true;
}

inline uint32_t HttpClient::Read(char* buffer,
                                 const uint32_t buffer_size) {
    /* Save the total received size */
    uint32_t total_received = 0;

    /* Read while we have a free space in the buffer */
    while (total_received < buffer_size) {
        /* Receve the data from the transport */
        const uint32_t free_space = buffer_size - total_received;
        const uint32_t received = _transport.Receive(buffer, free_space);

        /* If we get the real data */
        if (received > 0) {
            /* Update the pointer and the total received size */
            buffer         += received;
            total_received += received;
        }

        /* If the connection has been closed or there is an error
         * Return the result */
        else return total_received;
    }

    /* Return the result */
    return total_received;
}

inline uint16_t HttpClient::Port() const noexcept { return htons(80); }

inline void HttpClient::Abort() {
    /* Abort the current connection */
    _transport.Abort();

    /* Reset the state */
    _state = SENDING_HEADERS;
}
