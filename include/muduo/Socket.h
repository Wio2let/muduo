#pragma once

#include "muduo/noncopyable.h"

namespace muduo {

class InetAddress;

class Socket : noncopyable {
public:
    explicit Socket(int sockfd);
    ~Socket();

    int fd() const;

    void bindAddress(const InetAddress& localAddr);
    void listen();
    int accept(InetAddress* peerAddr);

    void setReuseAddr(bool on);
    void setReusePort(bool on);

private:
    const int sockfd_;
};

}  // namespace muduo
