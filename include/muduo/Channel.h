#pragma once

#include "muduo/Timestamp.h"
#include "muduo/noncopyable.h"

#include <functional>

namespace muduo {

class EventLoop;

class Channel : noncopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel() = default;

    void handleEvent(Timestamp receiveTime);

    void setReadCallback(ReadEventCallback cb);
    void setWriteCallback(EventCallback cb);
    void setCloseCallback(EventCallback cb);
    void setErrorCallback(EventCallback cb);

    int fd() const;
    int events() const;
    void setRevents(int revents);
    bool isNoneEvent() const;

    int index() const;
    void setIndex(int index);

    void enableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();

    EventLoop* ownerLoop() const;
    void remove();

private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_{0};
    int revents_{0};
    int index_{-1};

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

}  // namespace muduo
