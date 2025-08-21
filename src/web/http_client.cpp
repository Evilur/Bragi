#include "http_client.h"
#include "dns.h"
#include "exception/http_exception.h"
#include "template/linked_list.hpp"
#include "types/string.hpp"
#include "util/logger.hpp"

#include <cmath>
#include <cstring>
#include <iostream>
#include <unistd.h>

static unsigned int num_of_digits(unsigned int num) {
    unsigned int result = 0;
    do {
        result++;
        num >>= 4;
    } while (num != 0);
    return result;
}

HttpClient::HttpClient(const char* const hostname,
                       const char* const path,
                       const char* const headers,
                       const char* const method,
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

    /* Get the length of custom headers */
    const unsigned int custom_headers_size = strlen(headers);

    /* Assemble basic headers */
    const unsigned int request_headers_size = snprintf(
        _buffer,
        BUFFER_SIZE,
        custom_headers_size > 0 ?
        "%s /%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        :
        "%s /%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        "\r\n",
        method, path, hostname, request_body_size
    );

    /* Check for the response size */
    if (request_headers_size > BUFFER_SIZE) {
        FATAL_LOG("Failed to save the request into the buffer");
        throw HttpException();
    }

    /* Write the request headers to the socket */
    if (!Write(_buffer, request_headers_size)) {
        ERROR_LOG("Failed to send headers");
        throw HttpException();
    }

    /* Write custom headers */
    if (custom_headers_size > 0 &&
        (!Write(headers, custom_headers_size) ||
            !Write("\r\n\r\n", 4))) {
        ERROR_LOG("Failed to write custom headers");
        throw HttpException();
    }

    /* Write the full body to the socket */
    if (request_body_size > 0 && !Write(body, request_body_size)) {
        ERROR_LOG("Failed to send the body");
        throw HttpException();
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
            _reader = new CompleteReader(String::ToUInt64(content_length_str));
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

HttpClient::CompleteReader::CompleteReader(const unsigned long content_length)
    noexcept : _content_length(content_length) { }

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
        memcpy(out, http->_buffer + http->_buffer_offset, copy_size);

        /* Change pointer, offset and sized according to the copy size */
        out += copy_size;
        size -= copy_size;
        http->_buffer_offset += copy_size;
        http->_buffer_size -= copy_size;
        _content_length -= copy_size;
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
            _content_length -= read_result;
        }
    }

    /* Return the size */
    return original_size - size;
}

String HttpClient::CompleteReader::ReadAll(HttpClient* http) {
    /* Create a result buffer */
    String result(_content_length);
    char* out = (char*)(const char*)result;

    /* Drain the buffer to the result buffer */
    if (http->_buffer_size > 0) {
        mempcpy(out, http->_buffer + http->_buffer_offset, http->_buffer_size);
        out += http->_buffer_size;
        _content_length -= http->_buffer_size;
        http->_buffer_size = 0;
    }

    /* Read the data from the socket to the out buffer */
    while (_content_length > 0) {
        /* Try to read from the socket */
        if (const long read_result = read(http->_server_fd, out,
                                          _content_length);
            read_result <= 0) goto end;
        else _content_length -= read_result;
    }

    /* Set the eof to true and return the result */
    end:
    http->_eof = true;
    return result;
}

unsigned int HttpClient::ChunkedReader::Read(HttpClient* http,
                                             char* out,
                                             unsigned int size) { return 0; }

String HttpClient::ChunkedReader::ReadAll(HttpClient* http) {
    /* Create e linked list to store the data */
    class Chunk {
    public:
        const char* const data;
        unsigned int size;
    };
    LinkedList<const Chunk> chunks;

    /* Create a variable to store the complete size */
    unsigned long complete_size = 0;

    /* A pointer to the current chunk buffer */
    char* current_chunk;
    unsigned int current_chunk_size;

    /* Drain the buffer */
    while (http->_buffer_size > 0) {
        /* Get the size of a chunk */
        current_chunk_size =
            String::HexToInt(http->_buffer + http->_buffer_offset);
        if (current_chunk_size == 0) goto end;

        /* Get the size of the chunk size string */
        const unsigned int size_str_size = num_of_digits(current_chunk_size)
                                           + sizeof("\r\n") - 1;

        /* If the chunk is not full */
        if (size_str_size > http->_buffer_size ||
            http->_buffer_size < current_chunk_size) break;

        /* Eval the new buffer size and offset */
        http->_buffer_offset += size_str_size;
        http->_buffer_size -= size_str_size;

        /* Create a buffer */
        current_chunk = new char[current_chunk_size];
        chunks.Push({ current_chunk, current_chunk_size });
        complete_size += current_chunk_size;

        /* Copy the chunk to the buffer */
        memcpy(current_chunk,
               http->_buffer + http->_buffer_offset,
               current_chunk_size);

        /* Eval new sizes and offsets */
        current_chunk_size += 2;  // \r\n sequence
        http->_buffer_offset += current_chunk_size;
        http->_buffer_size -= current_chunk_size;
    }

    /* Move the buffer part to the start */
    if (http->_buffer_size > 0)
        memmove(http->_buffer,
                http->_buffer + http->_buffer_offset,
                http->_buffer_size);

    /* Read the data from the socket */
    for (;;) {
        /* Read the size string from the socket */
        http->_buffer_offset = 0;
        if (http->_buffer_size < sizeof("00000000\r\n")) {
            unsigned int free_space = sizeof("00000000\r\n") -
                                      http->_buffer_size;
            while (free_space > 0) {
                if (const long read_result = read(
                        http->_server_fd,
                        http->_buffer + http->_buffer_size,
                        free_space
                    ); read_result <= 0)
                        goto end;
                else {
                    free_space -= read_result;
                    http->_buffer_size += read_result;
                }
            }
        }

        /* Get the chunk size and allocate the memory */
        current_chunk_size = String::HexToInt(http->_buffer);
        if (current_chunk_size == 0)
            goto end;
        current_chunk = new char[current_chunk_size + 2];  // + '\r\n'
        chunks.Push({ current_chunk, current_chunk_size });
        complete_size += current_chunk_size;

        /* Get the size of the size string */
        const unsigned int size_str_size = num_of_digits(current_chunk_size) +
                                           sizeof("\r\n") - 1;

        /* Eval the new buffer size and offset */
        http->_buffer_offset += size_str_size;
        http->_buffer_size -= size_str_size;

        /* Copy the part of the buffer to the chunk */
        memcpy(current_chunk,
               http->_buffer + http->_buffer_offset,
               http->_buffer_size);
        current_chunk += http->_buffer_size;
        current_chunk_size -= http->_buffer_size;
        http->_buffer_size = 0;
        http->_buffer_offset = 0;

        /* Read a chunk to the buffer */
        current_chunk_size += 2;  // + '\r\n'
        while (current_chunk_size > 0) {
            if (const long read_size =
                    read(http->_server_fd, current_chunk, current_chunk_size);
                read_size <= 0) goto end;
            else {
                current_chunk += read_size;
                current_chunk_size -= read_size;
            }
        }
    }

    /* Create a String object to store the result */
    end:
    String result(complete_size);
    char* out = (char*)(const char*)result;

    for (const auto& [data, size] : chunks) {
        /* Copy the data from the chunk */
        memcpy(out, data, size);

        /* Increment the pointer */
        out += size;

        /* Free the memory */
        delete[] data;
    }

    return result;
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