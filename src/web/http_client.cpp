#include "http_client.h"
#include "dns.h"
#include "types/string.hpp"
#include "util/logger.hpp"

#include <cstring>
#include <iostream>
#include <unistd.h>

HttpClient::HttpClient(const char* const hostname, const char* const path,
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

    /* Assemble the request to send */
    const unsigned int request_headers_size = snprintf(
        _buffer,
        BUFFER_SIZE,
        "%s /%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        "%s"
        "\r\n",
        method, path, hostname, request_body_size, headers
    );

    /* Check for the response size */
    if (request_headers_size > BUFFER_SIZE) {
        ERROR_LOG("Failed to save the request into the buffer");
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

    /* Read the response */
    response_read:
    _buffer_size = 0;
    {
        /* Try to read the socket */
        const long read_result = read(_server_fd,
                                     _buffer + _buffer_size,
                                     BUFFER_SIZE - _buffer_size);
        if (read_result == -1) {
            ERROR_LOG("Failed to read the response");
            return;
        }

        /* Increase the buffer size */
        _buffer_size += read_result;
    }

    /* Read headers */
    const char* buffer_ptr = _buffer;
    bool read_all_headers = false;
    do {
        /* Get the header */
        buffer_ptr = strstr(buffer_ptr, "\r\n");
        if (buffer_ptr != nullptr) buffer_ptr += 2;
        else break;

        /* Handle Content-Length header */
        if (strncmp(buffer_ptr,
                    "Content-Length:",
                    sizeof("Content-Length")) == 0) {
            buffer_ptr = strchr(buffer_ptr, ':') + 1;
            while (*buffer_ptr < '0' || *buffer_ptr > '9') buffer_ptr++;
            _content_length = String::ToUInt64(buffer_ptr);
        /* Handle the empty line */
        } else if (strncmp(buffer_ptr, "\r\n", 2) == 0) {
            read_all_headers = true;
            buffer_ptr += 2;
            break;
        }
    } while (_buffer_size > 0);

    /* If we don't go through all the headers */
    if (!read_all_headers) goto response_read;

    /* If we've read all headers */
    _buffer_offset = buffer_ptr - _buffer;
    _buffer_size -= _buffer_offset;
}

HttpClient::~HttpClient() {
    close(_server_fd);
}

bool HttpClient::End() const { return _eof; }

unsigned int HttpClient::Read(char* out, unsigned int size) {
    /* Save the size variable value */
    const unsigned int firstborn_size = size;

    /* If we have the data in the buffer */
    if (_buffer_size) {
        /* Get the size of data to copy */
        const unsigned int cpy_size = _buffer_size < size ?
                                      _buffer_size : size;

        /* Copy the data from the buffer to the out array */
        memcpy(out, _buffer + _buffer_offset, cpy_size);
        out += cpy_size;
        size -= cpy_size;

        /* Increase the buffer offset */
        _buffer_offset += cpy_size;
        _buffer_size -= cpy_size;
    }

    /* Read the socket while there is a free space in the out array */
    for (unsigned int i = 0; i < size;) {
        if (const long read_result = read(_server_fd, out, size);
            read_result <= 0) {
            /* Set the eof boolean to true */
            _eof = true;

            /* Eval the size of written data and return it */
            return firstborn_size - size;
        } else i += read_result;
    }

    /* Return the firstborn size value */
    return firstborn_size;
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