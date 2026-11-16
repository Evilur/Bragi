#pragma once

#include "type/nullable.h"
#include "util/class.h"
#include "web/dns.h"
#include "web/http_request.h"
#include "web/http_response.h"
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

    bool HasResponse() const noexcept;

    const HttpResponse& Response() const noexcept;

    virtual bool Write(const char* buffer, uint32_t buffer_size);

    virtual bool Flush();

    virtual uint32_t Read(char* buffer, uint32_t buffer_size);

protected:
    virtual uint16_t Port() const noexcept;

private:
    HttpRequest _request;
    HttpTransport _transport;
    Nullable<HttpResponse> _response;

    bool _requestSent = false;
    bool _responseRecved = false;

    void Abort();
};

inline HttpClient::HttpClient(const char* const domain,
                              const char* const path,
                              const char* const method) noexcept
: _request(domain, path, method), _transport(DNS::Resolve(domain), Port()) { }

inline HttpClient::HttpClient(const HttpRequest& request) noexcept
: _request(request), _transport(DNS::Resolve(request.domain), Port()) { }

inline HttpRequest& HttpClient::Request() noexcept { return _request; }

inline bool HttpClient::HasResponse() const
noexcept { return _response != nullptr; }

inline const HttpResponse& HttpClient::Response() const noexcept {
    assert(HasResponse() && "No response received yet");
    return *_response;
}

inline bool HttpClient::Write(const char* const buffer,
                              const uint32_t buffer_size) {
    /* If we haven't sent the headers yet, do it now */
    if (!_requestSent) {
        /* Connect the transport to the server */
        try { _transport.Connect(); }
        catch (const SocketException& e)
        { throw HttpException("Failed to connect to the server"); }

        /* Send the request headers */
        _transport.SetTxEncoder(HttpEncoder::Plain::Instance());
        try { _request.SendHttp11Headers(_transport); }
        catch (const SocketException& e)
        { Abort(); return false; }

        /* Set the right encoder */
        if (_request.IsChunkedEncoding())
            _transport.SetTxEncoder(HttpEncoder::Chunked::Instance());
        else
            _transport.SetTxEncoder(HttpEncoder::Plain::Instance());

        /* Else reset the Content-Length */
        _request.ResetContentLength();

        /* Update the state */
        _requestSent = true;
    }

    /* Send the body */
    try { _transport.Write(buffer, buffer_size); }
    catch (const SocketException& e)
    { Abort(); return false; }

    /* Return the success code */
    return true;
}

inline bool HttpClient::Flush() {
    /* If we haven't sent the headers yet, do it now */
    if (!_requestSent) {
        /* Connect the transport to the server */
        try { _transport.Connect(); }
        catch (const SocketException& e)
        { throw HttpException("Failed to connect to the server"); }

        /* Set the 'Content-Length' to 0 and send the request headers */
        _request.SetContentLength(0);
        _transport.SetTxEncoder(HttpEncoder::Plain::Instance());
        try { _request.SendHttp11Headers(_transport); }
        catch (const SocketException& e)
        { Abort(); return false; }

        /* Reset the Content-Length */
        _request.ResetContentLength();
    }

    /* Flush the request */
    try { _transport.Flush(); }
    catch (const SocketException& e)
    { Abort(); return false; }

    /* Reset the request state */
    _requestSent = false;

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
        const uint32_t received = _transport.Read(buffer, free_space);

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

    /* Reset the states */
    _requestSent = false;
    _responseRecved = false;
}
