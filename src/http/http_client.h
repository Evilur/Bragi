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

    virtual void Write(const char* buffer, uint32_t buffer_size);

    virtual void Flush();

    virtual uint32_t Read(char* buffer, uint32_t buffer_size);

    void Close() noexcept;

private:
    HttpRequest _request;
    Nullable<HttpResponse> _response = nullptr;

    UniqPtr<HttpTxEncoder> _tx_encoder = nullptr;
    UniqPtr<HttpRxEncoder> _rx_encoder = nullptr;

    HttpTransport _transport;

    bool _headersSent = false;
};

inline HttpClient::HttpClient(const char* const domain,
                              const char* const path,
                              const char* const method)
: _request(domain, path, method),
  _transport(
      _tx_encoder.Ref(), _rx_encoder.Ref(),
      DNS::Resolve(domain), htons(80)
  ) { }

inline HttpClient::HttpClient(const HttpRequest& request)
: _request(request),
  _transport(
      _tx_encoder.Ref(), _rx_encoder.Ref(),
      DNS::Resolve(request.domain), htons(80)
  ) { }

inline HttpRequest& HttpClient::Request() noexcept { return _request; }

inline bool HttpClient::HasResponse() const
noexcept { return _response != nullptr; }

inline const HttpResponse& HttpClient::Response() const noexcept {
    assert(HasResponse() && "No response received yet");
    return *_response;
}

inline void HttpClient::Write(const char* const buffer,
                              const uint32_t buffer_size) {
    /* If we haven't sent the headers yet, do it now */
    if (!_headersSent) {
        /* Check for the transport state */
        if (_transport.IsClosed()) _transport.Reset();

        /* Connect the transport to the server */
        try { _transport.Connect(); }
        catch (const SocketException& e)
        { throw HttpException("Failed to connect to the server"); }

        /* Send the request headers */
        try { _request.SendHttp11Headers(_transport); }
        catch (const SocketException& e)
        { throw HttpException("Failed to send the http request headers"); }

        /* Set the right encoder */
        if (_request.IsChunkedEncoding())
            _tx_encoder = new HttpTxEncoder::Chunked();
        else
            _tx_encoder = new HttpTxEncoder::Plain(_request.GetContentLength());

        /* Else prepare for the next request */
        _request.ResetContentLength();

        /* Update the state */
        _headersSent = true;
    }

    /* Send the body */
    try { _transport.Write(buffer, buffer_size); }
    catch (const SocketException& e)
    { throw HttpException("Failed to send the http body"); }
}

inline void HttpClient::Flush() {
    /* If we haven't sent the headers yet, do it now */
    if (!_headersSent) {
        /* Check for the transport state */
        if (_transport.IsClosed()) _transport.Reset();

        /* Connect the transport to the server */
        try { _transport.Connect(); }
        catch (const SocketException& e)
        { throw HttpException("Failed to connect to the server"); }

        /* Set the 'Content-Length' to 0 and send the request headers */
        _request.SetContentLength(0);
        try { _request.SendHttp11Headers(_transport); }
        catch (const SocketException& e)
        { throw HttpException("Failed to send the http request headers"); }

        /* Prepare the request for the next request */
        _request.ResetContentLength();
    }

    /* Flush the request */
    try { _transport.Flush(); }
    catch (const SocketException& e)
    { throw HttpException("Failed to flush the http data"); }

    /* Reset the tx encoder */
    _tx_encoder = nullptr;

    /* Reset the request state */
    _headersSent = false;

    /* Create a new http response object */
    _response = HttpResponse();

    /* Read the response headers */
    try { _response->ReadHttp11Headers(_transport, _request.method); }
    catch (const SocketException& e)
    { throw HttpException("Failed to read the http response headers"); }

    /* Check for the response validity */
    if (!_response->IsValid())
        throw HttpException("The server returned invalid response");

    /* Set the right rx encoder */
    if (_response->IsChunkedEncoding())
        _rx_encoder = new HttpRxEncoder::Chunked();
    else
        _rx_encoder = new HttpRxEncoder::Plain(_response->ContentLength());
}

inline uint32_t HttpClient::Read(char* const buffer,
                                 const uint32_t buffer_size) {
    /* Check for the transport state and the buffer state */
    if (_transport.IsClosed() &&
        _transport.RxBufferedSize() == 0) return 0;

    /* Read the data */
    try { return _transport.Read(buffer, buffer_size); }
    catch (const SocketException& e)
    { throw HttpException("The connection has been closed"); }
}

inline void HttpClient::Close() noexcept { _transport.Close(); }
