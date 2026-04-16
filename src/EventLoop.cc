#include "muduo/EventLoop.h"

#include "muduo/Channel.h"

#include <stdexcept>

namespace muduo {

namespace {
thread_local EventLoop* loopInThisThread = nullptr;
constexpr int kPollTimeMs = 10000;
}

EventLoop::EventLoop()
    : threadId_(CurrentThread::tid()),
      poller_(Poller::newDefaultPoller(this)) {
    if (loopInThisThread != nullptr) {
        throw std::runtime_error("only one EventLoop is allowed per thread");
    }
    loopInThisThread = this;
}

EventLoop::~EventLoop() {
    loopInThisThread = nullptr;
}

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;

    while (!quit_) {
        activeChannels_.clear();
        const Timestamp pollReturnTime = poller_->poll(kPollTimeMs, &activeChannels_);

        for (Channel* channel : activeChannels_) {
            channel->handleEvent(pollReturnTime);
        }
    }

    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
}

void EventLoop::updateChannel(Channel* channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) const {
    return poller_->hasChannel(channel);
}

bool EventLoop::isInLoopThread() const {
    return threadId_ == CurrentThread::tid();
}

}  // namespace muduo
