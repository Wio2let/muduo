#include "muduo/Channel.h"

#include "muduo/EventLoop.h"

#include <sys/epoll.h>

namespace muduo {

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop), fd_(fd) {}

void Channel::handleEvent(Timestamp receiveTime) {
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (revents_ & EPOLLERR) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}

void Channel::setReadCallback(ReadEventCallback cb) {
    readCallback_ = std::move(cb);
}

void Channel::setWriteCallback(EventCallback cb) {
    writeCallback_ = std::move(cb);
}

void Channel::setCloseCallback(EventCallback cb) {
    closeCallback_ = std::move(cb);
}

void Channel::setErrorCallback(EventCallback cb) {
    errorCallback_ = std::move(cb);
}

int Channel::fd() const {
    return fd_;
}

int Channel::events() const {
    return events_;
}

void Channel::setRevents(int revents) {
    revents_ = revents;
}

bool Channel::isNoneEvent() const {
    return events_ == kNoneEvent;
}

int Channel::index() const {
    return index_;
}

void Channel::setIndex(int index) {
    index_ = index;
}

void Channel::enableReading() {
    events_ |= kReadEvent;
    update();
}

void Channel::enableWriting() {
    events_ |= kWriteEvent;
    update();
}

void Channel::disableWriting() {
    events_ &= ~kWriteEvent;
    update();
}

void Channel::disableAll() {
    events_ = kNoneEvent;
    update();
}

EventLoop* Channel::ownerLoop() const {
    return loop_;
}

void Channel::remove() {
    loop_->removeChannel(this);
}

void Channel::update() {
    loop_->updateChannel(this);
}

}  // namespace muduo
