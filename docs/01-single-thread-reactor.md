# Stage 1: 单线程 Reactor

这一阶段先实现最小单线程 Reactor。

目标是让一个线程里的 `EventLoop` 监听 fd，并在事件发生时调用回调。

## 三个核心类

```text
             owns
EventLoop ----------> Poller
    |
    | dispatches active channels
    v
 Channel
    |
    | wraps
    v
   fd
```

执行流程：

```text
EventLoop::loop()
    |
    v
Poller::poll()
    |
    v
epoll_wait()
    |
    v
active Channel list
    |
    v
Channel::handleEvent()
    |
    v
read/write/close/error callback
```

## 设计拆分

如果 `EventLoop` 直接管理 fd，它就会同时负责：

- 等事件
- 记录每个 fd 关注什么事件
- 记录每个 fd 对应什么回调
- 分发事件

这会让 `EventLoop` 过重。

这里采用的职责划分是：

- `EventLoop` 管循环
- `Poller` 管 IO 多路复用
- `Channel` 管一个 fd 的事件和回调

## 本阶段要写的接口

```cpp
class Channel {
public:
    void setReadCallback(ReadEventCallback cb);
    void setWriteCallback(EventCallback cb);

    void enableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();

    void handleEvent(Timestamp receiveTime);
};
```

```cpp
class Poller {
public:
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
};
```

```cpp
class EventLoop {
public:
    void loop();
    void quit();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
};
```

## 最小验收

这一阶段的验证方式是写一个 pipe 或 eventfd 示例：

```text
write pipe/eventfd
        |
        v
epoll detects readable event
        |
        v
Channel read callback runs
        |
        v
EventLoop quits
```

这个例子跑通后，再继续接入 socket 会更直接。
