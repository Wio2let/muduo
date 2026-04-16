#include "muduo/EPollPoller.h"

#include "muduo/Channel.h"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <unistd.h>

namespace muduo {

namespace {
constexpr int kNew = -1;
constexpr int kAdded = 1;
constexpr int kDeleted = 2;
constexpr int kInitEventListSize = 16;
}

EPollPoller::EPollPoller(EventLoop* loop)
    : Poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize) {
    if (epollfd_ < 0) {
        throw std::runtime_error("epoll_create1 failed");
    }
}

EPollPoller::~EPollPoller() {
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels) {
    const int numEvents =
        ::epoll_wait(epollfd_, events_.data(), static_cast<int>(events_.size()), timeoutMs);
    const Timestamp now = Timestamp::now();

    if (numEvents > 0) {
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents < 0 && errno != EINTR) {
        throw std::runtime_error(std::string("epoll_wait failed: ") + std::strerror(errno));
    }

    return now;
}

void EPollPoller::updateChannel(Channel* channel) {
    const int index = channel->index();

    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            channels_[channel->fd()] = channel;
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
        return;
    }

    if (channel->isNoneEvent()) {
        update(EPOLL_CTL_DEL, channel);
        channel->setIndex(kDeleted);
    } else {
        update(EPOLL_CTL_MOD, channel);
    }
}

void EPollPoller::removeChannel(Channel* channel) {
    const int fd = channel->fd();
    channels_.erase(fd);

    if (channel->index() == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    for (int i = 0; i < numEvents; ++i) {
        auto* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::update(int operation, Channel* channel) {
    epoll_event event;
    std::memset(&event, 0, sizeof event);

    event.events = channel->events();
    event.data.ptr = channel;

    if (::epoll_ctl(epollfd_, operation, channel->fd(), &event) < 0) {
        throw std::runtime_error(std::string("epoll_ctl failed: ") + std::strerror(errno));
    }
}

}  // namespace muduo
