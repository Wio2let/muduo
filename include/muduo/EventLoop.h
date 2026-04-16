#pragma once

#include "muduo/CurrentThread.h"
#include "muduo/Poller.h"
#include "muduo/Timestamp.h"
#include "muduo/noncopyable.h"

#include <atomic>
#include <memory>
#include <vector>

namespace muduo {

class Channel;

class EventLoop : noncopyable {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel) const;

    bool isInLoopThread() const;

private:
    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_{false};
    std::atomic_bool quit_{false};
    const pid_t threadId_;
    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;
};

}  // namespace muduo
