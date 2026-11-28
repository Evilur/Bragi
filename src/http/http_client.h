#pragma once

#include "type/nullable.h"
#include "type/uniq_ptr.h"
#include "util/class_traits.h"
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

    [[nodiscard]]
    HttpRequest& Request() noexcept;

    [[nodiscard]]
    bool HasResponse() const noexcept;

    [[nodiscard]]
    const HttpResponse& Response() const noexcept;

    virtual void Write(const char* buffer, uint32_t buffer_size);

    virtual void Flush();

    [[nodiscard]]
    virtual uint32_t Read(char* buffer, uint32_t buffer_size);

    void Close() noexcept;

private:
    HttpRequest _request;
    Nullable<HttpResponse> _response = nullptr;

    UniqPtr<HttpTxEncoder> _tx_encoder = nullptr;
    UniqPtr<HttpRxEncoder> _rx_encoder = nullptr;

    HttpTransport _transport;

    bool _headersSent = false;

    bool _is_connection_close = true;

    bool _connected = false;
    bool _reusable  = false;

    void EnsureConnected();
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
    assert(HasResponse());
    return *_response;
}

inline void HttpClient::Write(const char* const buffer,
                              const uint32_t buffer_size) {
    try {
        /* If we haven't sent the headers yet, do it now */
        if (!_headersSent) {
            /* Prepare for the next request */
            EnsureConnected();

            /* Send the request headers */
            try { _request.SendHttp11Headers(_transport); }
            catch (const SocketException& e)
            { throw HttpException("Failed to send the http request headers"); }

            /* Set the right encoder */
            if (_request.IsChunkedEncoding())
                _tx_encoder = new HttpTxEncoder::Chunked();
            else
                _tx_encoder = new HttpTxEncoder::Plain(_request.ContentLength());

            /* Else prepare for the next request */
            _request.ResetContentLength();

            /* Save the connection state */
            _is_connection_close = _request.close_connection;

            /* Update the state */
            _headersSent = true;
        }

        /* Send the body */
        try { _transport.Write(buffer, buffer_size); }
        catch (const SocketException& e)
        { throw HttpException("Failed to send the http body"); }
    } catch (const HttpException& e) { Close(); throw; }
}

inline void HttpClient::Flush() {
    try {
        /* If we haven't sent the headers yet, do it now */
        if (!_headersSent) {
            /* Prepare for the next request */
            EnsureConnected();

            /* Set the 'Content-Length' to 0 and send the request headers */
            _request.SetContentLength(0);
            try { _request.SendHttp11Headers(_transport); }
            catch (const SocketException& e)
            { throw HttpException("Failed to send the http request headers"); }

            /* Prepare the request for the next request */
            _request.ResetContentLength();

            /* Save the connection state */
            _is_connection_close = _request.close_connection;
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
        else if (_response->IsChunkedEncoding()) {
            _rx_encoder = new HttpRxEncoder::Chunked();
            _reusable =
                !_is_connection_close && !_response->IsConnectionClose();
        } else if (const Nullable<uint64_t>& content_length =
                 _response->ContentLength(); content_length != nullptr) {
            _rx_encoder = new HttpRxEncoder::Plain(*content_length);
            _reusable =
                !_is_connection_close && !_response->IsConnectionClose();
        } else
            _rx_encoder = new HttpRxEncoder::Plain();
    } catch (const HttpException& e) { Close(); throw; }
}

inline uint32_t HttpClient::Read(char* const buffer,
                                 const uint32_t buffer_size) {
    try {
        /* Check for the transport state and the buffer state */
        if (_transport.IsClosed() &&
            _transport.RxBufferedSize() == 0) return 0;
        assert(_rx_encoder.Get() != nullptr);

        /* Read the data */
        try { return _transport.Read(buffer, buffer_size); }
        catch (const SocketException& e)
        { throw HttpException("The connection has been closed"); }
    } catch (const HttpException& e) { Close(); throw; }
}

inline void HttpClient::Close() noexcept {
    _transport.Close();
    _connected = false;
    _reusable = false;
    _headersSent = false;
    _tx_encoder = nullptr;
    _rx_encoder = nullptr;
    _response = nullptr;
}

inline void HttpClient::EnsureConnected() {
    /* Use the old connection only if the connection is ON
     * And there is no body to drain */
    if (_connected && !(
            _connected && _reusable &&
            !_transport.IsClosed() &&
            _rx_encoder.Get() != nullptr && _rx_encoder->IsFinished() &&
            _transport.RxBufferedSize() == 0
        )) {
        _transport.Close();
        _connected = false;
    }

    /* Reset the state */
    _reusable = false;

    /* If we already connected */
    if (_connected) return;

    /* If we need to connect */
    if (_transport.IsClosed()) _transport.Reset();
    try { _transport.Connect(); }
    catch (const SocketException& e)
    { throw HttpException("Failed to connect to the server"); }
    _connected = true;
}
