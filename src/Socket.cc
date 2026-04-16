#include "muduo/Socket.h"

#include "muduo/InetAddress.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace muduo {

Socket::Socket(int sockfd)
    : sockfd_(sockfd) {}

Socket::~Socket() {
    ::close(sockfd_);
}

int Socket::fd() const {
    return sockfd_;
}

void Socket::bindAddress(const InetAddress& localAddr) {
    const int ret = ::bind(
        sockfd_,
        reinterpret_cast<const sockaddr*>(localAddr.getSockAddr()),
        sizeof(sockaddr_in));
    if (ret < 0) {
        throw std::runtime_error(std::string("bind failed: ") + std::strerror(errno));
    }
}

void Socket::listen() {
    if (::listen(sockfd_, SOMAXCONN) < 0) {
        throw std::runtime_error(std::string("listen failed: ") + std::strerror(errno));
    }
}

int Socket::accept(InetAddress* peerAddr) {
    sockaddr_in addr;
    socklen_t len = sizeof addr;
    const int connfd = ::accept4(
        sockfd_,
        reinterpret_cast<sockaddr*>(&addr),
        &len,
        SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (connfd >= 0) {
        peerAddr->setSockAddr(addr);
    }

    return connfd;
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}

void Socket::setReusePort(bool on) {
#ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);
#else
    (void)on;
#endif
}

}  // namespace muduo
