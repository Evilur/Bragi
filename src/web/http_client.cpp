#include "http_client.h"
#include "dns.h"
#include "template/linked_list.hpp"
#include "types/string.hpp"
#include "util/logger.hpp"

#include <cstring>
#include <iostream>
#include <unistd.h>

HttpClient::HttpClient(const char* const hostname,
                       const char* const path,
                       const char* const headers, const char* const method,
                       const char* const body) {
    /* Create a socket */
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1) {
        ERROR_LOG("Failed to create a socket");
        return;
    }

    /* Create a server address struct */
    const sockaddr_in server_address {
        .sin_family = AF_INET,
        .sin_port = _port,
        .sin_addr = { DNS::Resolve(hostname) }
    };

    /* Connect to the server */
    if (connect(_server_fd, (sockaddr*)&server_address, sizeof(server_address))
        == -1) {
        ERROR_LOG("Failed to connect to the server");
        return;
    }

    /* Log the message */
    TRACE_LOG("Successfully connected to the server");

    /* Get the length of the body */
    const unsigned int request_body_size = strlen(body);

    /* Assemble basic headers */
    const unsigned int request_headers_size = snprintf(
        _buffer,
        BUFFER_SIZE,
        "%s /%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "%s"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        "\r\n",
        method, path, hostname, headers, request_body_size
    );

    /* Check for the response size */
    if (request_headers_size > BUFFER_SIZE) {
        FATAL_LOG("Failed to save the request into the buffer");
        return;
    }

    /* Write the request headers to the socket */
    if (!Write(_buffer, request_headers_size)) {
        ERROR_LOG("Failed to send headers");
        return;
    }

    /* Write the full body to the socket */
    if (request_body_size && !Write(body, request_body_size)) {
        ERROR_LOG("Failed to send the body");
        return;
    }

    /* Read headers from the socket */
read_data:
    _buffer_size += read(_server_fd,
                         _buffer + _buffer_offset,
                         BUFFER_SIZE - _buffer_offset);

    /* Handle all headers */
    const char* header = _buffer;
    while (_buffer_size > 0) {
        /* Check for response status header */
        if (strncmp(header, "HTTP/", sizeof("HTTP/") - 1) == 0) {
            /* Get the status code */
            const char* status_code_str =
                (char*)memchr(header, ' ', _buffer_size);
            if (status_code_str == nullptr) break;
            _status_code = String::ToUInt16(status_code_str + 1);
        /* Check for the Content-Length header */
        } else if (strncmp(header,
                           "Content-Length:",
                           sizeof("Content-Length:") - 1) == 0) {
            /* Get the content length */
            const char* content_length_str =
                header + sizeof("Content-Length:");
            while (*content_length_str < '0' || *content_length_str > '9')
                content_length_str++;
            _content_length = String::ToUInt64(content_length_str);
            _reader = new CompleteReader();
        /* Check for the eof */
        } else if (strncmp(header, "\r\n", sizeof("\r\n") -1) == 0) {
            /* Get the body */
            const char* body_str = header + sizeof("\r\n") - 1;

            /* Set the buffer offset and exit */
            _buffer_offset = body_str - _buffer;
            _buffer_size -= sizeof("\r\n") - 1;

            /* If the _reader field is not initialized */
            if (!_reader) _reader = new ChunkedReader();
            return;
        }

        /* Get the next header */
        const char* next_header =
            (char*)memmem(header, _buffer_size, "\r\n", sizeof("\r\n") - 1);
        if (next_header == nullptr) break;
        next_header += sizeof("\r\n") - 1;
        _buffer_size -= next_header - header;
        header = next_header;
    }

    /* If we've not read all headers, copy the last one to the buffer */
    memmove(_buffer, header, _buffer_size);
    _buffer_offset = _buffer_size;

    /* Then read new headers from the socket */
    goto read_data;
}

HttpClient::~HttpClient() {
    close(_server_fd);
    delete _reader;
}

bool HttpClient::End() const { return _eof; }

unsigned int HttpClient::Read(char* const out, const unsigned int size) {
    return _reader->Read(this, out, size);
}

String HttpClient::ReadAll() {
    return _reader->ReadAll(this);
}

unsigned int HttpClient::CompleteReader::Read(HttpClient* http,
                                              char* out,
                                              unsigned int size) {
    /* Save the original out array size */
    const unsigned int original_size = size;

    /* If we have a data in the buffer */
    if (http->_buffer_size > 0) {
        /* Copy the data from the buffer */
        const unsigned int copy_size = http->_buffer_size < size ?
                                       http->_buffer_size : size;
        mempcpy(out, http->_buffer + http->_buffer_offset, copy_size);

        /* Change pointer, offset and sized according to the copy size */
        out += copy_size;
        size -= copy_size;
        http->_buffer_offset += copy_size;
        http->_buffer_size -= copy_size;
        http->_content_length -= copy_size;
    }

    /* Read the data from the socket */
    while (size > 0) {
        /* Try to read from the socket */
        if (const long read_result = read(http->_server_fd, out, size);
            read_result <= 0) {
            /* Set the eof boolean to true */
            http->_eof = true;

            /* Eval the size of written data and return it */
            return original_size - size;
        } else {
            size -= read_result;
            http->_content_length -= read_result;
        }
    }

    /* Return the size */
    return original_size - size;
}

String HttpClient::CompleteReader::ReadAll(HttpClient* http) {
    return "Placeholder";
}

unsigned int HttpClient::ChunkedReader::Read(HttpClient* http,
                                             char* out,
                                             unsigned int size) { return 0; }

String HttpClient::ChunkedReader::ReadAll(HttpClient* http) {
    return "Placeholder";
}

bool HttpClient::Write(const char* const buffer,
                       const unsigned int buffer_size) const {
    for (unsigned int i = 0; i < buffer_size;) {
        if (const long write_result =
                write(_server_fd, buffer + i, buffer_size - i);
            write_result == -1)
            return false;
        else i += write_result;
    }
    return true;
}