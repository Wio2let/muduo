#include "muduo/Channel.h"
#include "muduo/EventLoop.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace {

int createEventfd() {
    const int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        throw std::runtime_error("eventfd failed");
    }
    return fd;
}

}  // namespace

int main() {
    muduo::EventLoop loop;
    const int fd = createEventfd();
    muduo::Channel channel(&loop, fd);

    channel.setReadCallback([&](muduo::Timestamp receiveTime) {
        uint64_t value = 0;
        const ssize_t n = ::read(fd, &value, sizeof value);
        if (n != sizeof value) {
            throw std::runtime_error("eventfd read failed");
        }

        std::cout << "eventfd readable\n";
        std::cout << "value: " << value << '\n';
        std::cout << "receive time(us): " << receiveTime.toString() << '\n';

        channel.disableAll();
        channel.remove();
        loop.quit();
    });

    channel.enableReading();

    const uint64_t one = 1;
    const ssize_t n = ::write(fd, &one, sizeof one);
    if (n != sizeof one) {
        throw std::runtime_error("eventfd write failed");
    }

    loop.loop();
    ::close(fd);

    return 0;
}
