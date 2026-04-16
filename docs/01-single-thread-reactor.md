# Stage 1: 单线程 Reactor

这一阶段开始进入 muduo 的核心。

先不要想 `TcpServer`，也不要想线程池。我们只做一件事：

让一个线程里的 `EventLoop` 能监听 fd，并在事件发生时调用用户回调。

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

更贴近执行流程的图：

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

## 为什么不是 EventLoop 直接管理 fd

如果 `EventLoop` 直接管理 fd，它就会同时负责：

- 等事件
- 记录每个 fd 关注什么事件
- 记录每个 fd 对应什么回调
- 分发事件

这会让 `EventLoop` 过重。

muduo 的拆法更清楚：

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

这一阶段完成后，我们会写一个 pipe 或 eventfd 示例：

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

能跑通这个，再去写 socket 服务器就会顺很多。
