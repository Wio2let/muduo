#include "muduo/Acceptor.h"
#include "muduo/EventLoop.h"
#include "muduo/InetAddress.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace {

void connectOnce(uint16_t port) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    const int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    if (fd < 0) {
        throw std::runtime_error("client socket failed");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = ::htons(port);
    ::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof addr) < 0) {
        ::close(fd);
        throw std::runtime_error("client connect failed");
    }

    ::close(fd);
}

}  // namespace

int main() {
    constexpr uint16_t kListenPort = 20080;

    muduo::EventLoop loop;
    muduo::InetAddress listenAddr(kListenPort);
    muduo::Acceptor acceptor(&loop, listenAddr, true);

    acceptor.setNewConnectionCallback([&](int connfd, const muduo::InetAddress& peerAddr) {
        std::cout << "accepted connection from " << peerAddr.toIpPort() << '\n';
        ::close(connfd);
        loop.quit();
    });

    acceptor.listen();

    std::thread client(connectOnce, kListenPort);
    loop.loop();
    client.join();

    return 0;
}
