#include "muduo/Poller.h"

#include "muduo/Channel.h"
#include "muduo/EPollPoller.h"

namespace muduo {

Poller::Poller(EventLoop* loop)
    : ownerLoop_(loop) {}

bool Poller::hasChannel(Channel* channel) const {
    const auto it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

std::unique_ptr<Poller> Poller::newDefaultPoller(EventLoop* loop) {
    return std::make_unique<EPollPoller>(loop);
}

}  // namespace muduo
