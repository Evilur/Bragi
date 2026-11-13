#pragma once

#include "socket/socket_exception.h"
#include "util/class.h"
#include "util/logger.h"

#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

/**
 * A wrapper for the C sockets
 * @author Evilur <the.evilur@gmail.com>
 */
class TcpSocket final {
public:
    PREVENT_COPY_AND_MOVE(TcpSocket);

    TcpSocket();

    ~TcpSocket() noexcept;

    void Connect(const sockaddr_in& address);

    void Close() noexcept;

    int32_t Send(const char* buffer, uint32_t buffer_size) const noexcept;

    int32_t Receive(char* buffer, uint32_t buffer_size) const noexcept;

private:
    int32_t _socket_fd = -1;
};

inline TcpSocket::TcpSocket() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        ERROR_LOG("Can't startup the WSA");
        throw SocketException();
    }
#endif

    /* Create a socket */
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    /* Check the socket */
    if (_socket_fd == -1) {
        ERROR_LOG("Can't create a socket");
#ifdef _WIN32
        WSACleanup();
#endif
        throw SocketException();
    }
}

inline TcpSocket::~TcpSocket() noexcept { Close(); }

inline void TcpSocket::Connect(const sockaddr_in& address) {
    /* Connect to the address */
    if (connect(_socket_fd, (const sockaddr*)&address, sizeof(address)) ==
        -1) {
        ERROR_LOG("Can't connect to the server");
        Close();
        throw SocketException();
    }

    /* Print the connect address */
    INFO_LOG("Connect to %s:%hu",
             inet_ntoa(address.sin_addr),
             ntohs(address.sin_port));
}

inline void TcpSocket::Close() noexcept {
    if (_socket_fd == -1) return;
#ifdef _WIN32
    closesocket(_socket_fd);
    WSACleanup();
#else
    close(_socket_fd);
#endif
    _socket_fd = -1;
}

inline int32_t TcpSocket::Send(const char* const buffer,
                               const uint32_t buffer_size) const noexcept {
    /* Send the data */
    const int32_t result = send(_socket_fd, buffer, buffer_size, 0);

    /* Print the log */
#if LOG_LEVEL == 0
    sockaddr_in address;
    socklen_t address_len = sizeof(address);
    getpeername(_socket_fd, (sockaddr*)&address, &address_len);
    if (result != -1)
        TRACE_LOG("Send %d bytes to %s:%hu",
                  result,
                  inet_ntoa(address.sin_addr),
                  ntohs(address.sin_port));
    else WARN_LOG("Failed to send %ld bytes to %s:%hu",
                  buffer_size,
                  inet_ntoa(address.sin_addr),
                  ntohs(address.sin_port));
#endif

    /* Return the result */
    return result;
}

inline int32_t TcpSocket::Receive(char* buffer,
                                  const uint32_t buffer_size) const noexcept {
    /* Receive the data */
    const int32_t result = recv(_socket_fd, buffer, buffer_size, 0);

    /* Print the log */
#if LOG_LEVEL == 0
    if (result != -1) {
        sockaddr_in from;
        socklen_t from_len = sizeof(from);
        getpeername(_socket_fd, (sockaddr*)&from, &from_len);
        TRACE_LOG("Get %d bytes from %s:%hu",
                  result,
                  inet_ntoa(from.sin_addr),
                  ntohs(from.sin_port));
    }
#endif

    /* Return the result */
    return result;
}
