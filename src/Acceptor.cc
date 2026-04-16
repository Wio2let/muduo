#include "muduo/Acceptor.h"

#include "muduo/InetAddress.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace muduo {

namespace {

int createNonblockingSocket() {
    const int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0) {
        throw std::runtime_error(std::string("socket failed: ") + std::strerror(errno));
    }
    return sockfd;
}

}  // namespace

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort)
    : loop_(loop),
      acceptSocket_(createNonblockingSocket()),
      acceptChannel_(loop, acceptSocket_.fd()) {
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reusePort);
    acceptSocket_.bindAddress(listenAddr);

    acceptChannel_.setReadCallback([this](Timestamp) {
        handleRead();
    });
}

Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::setNewConnectionCallback(NewConnectionCallback cb) {
    newConnectionCallback_ = std::move(cb);
}

bool Acceptor::listening() const {
    return listening_;
}

void Acceptor::listen() {
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
    InetAddress peerAddr;
    const int connfd = acceptSocket_.accept(&peerAddr);

    if (connfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddr);
        } else {
            ::close(connfd);
        }
        return;
    }

    if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
        throw std::runtime_error(std::string("accept failed: ") + std::strerror(errno));
    }
}

}  // namespace muduo
