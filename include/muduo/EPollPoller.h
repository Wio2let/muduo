#pragma once

#include "muduo/Poller.h"

#include <sys/epoll.h>

#include <vector>

namespace muduo {

class EPollPoller : public Poller {
public:
    explicit EPollPoller(EventLoop* loop);
    ~EPollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:
    using EventList = std::vector<epoll_event>;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);

    int epollfd_;
    EventList events_;
};

}  // namespace muduo
