#pragma once

#include "type/nullable.h"
#include "type/uniq_ptr.h"
#include "util/class.h"
#include "http/dns.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_transport.h"

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
               const char* method = "GET");

    HttpClient(const HttpRequest& request);

    virtual ~HttpClient() noexcept = default;

    HttpRequest& Request() noexcept;

    bool HasResponse() const noexcept;

    const HttpResponse& Response() const noexcept;

    virtual bool Write(const char* buffer, uint32_t buffer_size);

    virtual bool Flush();

    virtual uint32_t Read(char* buffer, uint32_t buffer_size);

    void Close() noexcept;

    void Reset() noexcept;

private:
    HttpRequest _request;
    Nullable<HttpResponse> _response = nullptr;

    HttpTxEncoder* _tx_encoder = nullptr;
    UniqPtr<HttpRxEncoder> _rx_encoder = nullptr;

    HttpTransport _transport;

    Nullable<uint64_t> _content_length = nullptr;

    bool _headersSent = false;

    void Abort();
};

inline HttpClient::HttpClient(const char* const domain,
                              const char* const path,
                              const char* const method)
: _request(domain, path, method),
  _transport(
      _tx_encoder, _rx_encoder.Ref(), DNS::Resolve(domain), htons(80)
  ) { }

inline HttpClient::HttpClient(const HttpRequest& request)
: _request(request),
  _transport(
      _tx_encoder, _rx_encoder.Ref(), DNS::Resolve(request.domain), htons(80)
  ) { }

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
    if (!_headersSent) {
        /* Connect the transport to the server */
        try { _transport.Connect(); }
        catch (const SocketException& e)
        { throw HttpException("Failed to connect to the server"); }

        /* Send the request headers */
        try { _request.SendHttp11Headers(_transport); }
        catch (const SocketException& e)
        { Abort(); return false; }

        /* Set the right encoder */
        if (_request.IsChunkedEncoding())
            _tx_encoder = &HttpTxEncoder::Chunked::Instance();
        else
            _tx_encoder = &HttpTxEncoder::Plain::Instance();

        /* Try to save the content length */
        _content_length = _request.GetContentLength();

        /* Else prepare for the next request */
        _request.PrepareNextRequest();

        /* Update the state */
        _headersSent = true;
    }

    /* Check for the too large body */
    if (buffer_size > *_content_length)
        throw HttpException("The sent body is too large");
    else *_content_length -= buffer_size;

    /* Send the body */
    try { _transport.Write(buffer, buffer_size); }
    catch (const SocketException& e)
    { Abort(); return false; }

    /* Return the success code */
    return true;
}

inline bool HttpClient::Flush() {
    /* If we haven't sent the headers yet, do it now */
    if (!_headersSent) {
        /* Connect the transport to the server */
        try { _transport.Connect(); }
        catch (const SocketException& e)
        { throw HttpException("Failed to connect to the server"); }

        /* Set the 'Content-Length' to 0 and send the request headers */
        _request.SetContentLength(0);
        try { _request.SendHttp11Headers(_transport); }
        catch (const SocketException& e)
        { Abort(); return false; }

        /* Prepare the request for the next request */
        _request.PrepareNextRequest();

        /* Set the right encoder */
        _tx_encoder = &HttpTxEncoder::Plain::Instance();
    }

    /* Flush the request */
    try { _transport.Flush(); }
    catch (const SocketException& e)
    { Abort(); return false; }

    /* Reset the request state */
    _headersSent = false;

    /* Create a new http response object */
    _response = HttpResponse();

    /* Read the response headers */
    try { _response->ReadHttp11Headers(_transport); }
    catch (const SocketException& e)
    { Abort(); return false; }

    /* Check for the response validity */
    if (!_response->IsValid()) { Abort(); return false; }

    /* Set the right rx encoder */
    if (_response->IsChunkedEncoding())
        _rx_encoder = new HttpRxEncoder::Chunked();
    else
        _rx_encoder = new HttpRxEncoder::Plain(_response->ContentLength());

    /* If all is OK, return the success code */
    return true;
}

inline uint32_t HttpClient::Read(char* const buffer,
                                 const uint32_t buffer_size) {
    return _transport.Read(buffer, buffer_size);
}

inline void HttpClient::Close() noexcept { _transport.Close(); }

inline void HttpClient::Reset() noexcept {
    /* Reset the states */
    _headersSent = false;

    /* Reset the encoders */
    _tx_encoder = nullptr;
    _rx_encoder = nullptr;
}

inline void HttpClient::Abort() {
    /* Reset the current connection */
    _transport.Reset();

    /* Reset the client */
    Reset();
}
