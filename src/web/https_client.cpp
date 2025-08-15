#include "https_client.h"

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>

HttpsClient::HttpsClient(const std::string &url, const std::string &headers, const std::string &body, const char* requset_type) {
        /* Get the hostname and the get request from the url */
        const std::string request = url.starts_with("https://") ? url.substr(8) : url;
        const int get_index = request.find('/');
        const std::string host = request.substr(0, get_index);
        const std::string get = request.substr(get_index);

        /* Init the socket connection */
        asio::io_context svc;
        asio::ssl::context ctx(asio::ssl::context::method::sslv23_client);
        asio::ssl::stream <asio::ip::tcp::socket> ssock(svc, ctx);
        asio::ip::tcp::resolver resolver(svc);
        auto it = resolver.resolve(host, "443");
        asio::connect(ssock.lowest_layer(), it);
        ssock.handshake(asio::ssl::stream_base::handshake_type::client);

        /* Init the request */
        std::stringstream request_data;
        request_data << requset_type << ' ' << get << " HTTP/1.1\n"
                     << "Host: " << host << '\n'
                     << "Connection: close\n";

        /* Add custom headers (if exists) */
        if (!headers.empty()) request_data << headers << '\n';

        if (!body.empty()) {
                request_data << "Content-Length: " << body.length() << '\n'
                             << "\r\n" << body;
        }

        /* Write an empty line for the http request */
        request_data << "\r\n";

        /* Send the request */
        asio::write(ssock, asio::buffer(request_data.str()));

        std::stringstream response;
        asio::error_code ec;

        do {
                char buf[1024];
                size_t bytes_transferred = ssock.read_some(asio::buffer(buf), ec);
                if (!ec) response.write(buf, bytes_transferred);
        } while (!ec);

        char line_buf[1024];
        while (line_buf[0] != '\r') response.getline(line_buf, 1024);

        const int data_size = response.tellp() - response.tellg();
        _data = new char[data_size + 1];
        response.get(_data, data_size + 1);
}

const char* HttpsClient::ReadAll() { return _data; }
