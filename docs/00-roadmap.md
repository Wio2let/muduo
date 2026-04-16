# 复刻路线

这份文档是我们之后每一轮推进时的总导航。

## 整体结构

先把脑子里那张图立住：

```text
                   user code
                      |
                      v
                 +----------+
                 | TcpServer|
                 +----------+
                   |      |
                   |      v
                   |  +--------+
                   |  |Acceptor|
                   |  +--------+
                   |
                   v
             +-------------+
             |TcpConnection|
             +-------------+
               |    |    |
               |    |    v
               |    |  +------+
               |    |  |Buffer|
               |    |  +------+
               |    |
               |    v
               |  +-------+
               |  |Socket |
               |  +-------+
               |
               v
            +--------+        +-------------+
            |Channel | <----> |  EventLoop  |
            +--------+        +-------------+
                ^                    |
                |                    v
                +--------------> +--------+
                                 | Poller |
                                 +--------+
                                      |
                                      v
                                 +--------+
                                 | epoll  |
                                 +--------+
```

## 主流程

单线程版本先只抓这一条：

```text
socket readable
    |
    v
epoll_wait
    |
    v
Poller returns active channels
    |
    v
EventLoop dispatches
    |
    v
Channel invokes callback
    |
    v
TcpConnection / Acceptor handles event
```

多线程版本再在上面补这一条：

```text
main loop accepts connfd
        |
        v
choose sub loop
        |
        v
queue task to sub loop
        |
        v
wakeup sub loop by eventfd
```

## 分阶段目标

### Stage 0

目标：

- 建立工程骨架
- 写基础公共组件
- 保证构建链路正常

### Stage 1

目标：

- 让一个 `EventLoop` 能跑起来
- 让 `epoll_wait` 能返回活跃事件
- 让 `Channel` 能把事件分发到回调

完成后你应该能回答：

- `Channel` 为什么不直接等于 fd？
- `EventLoop` 和 `Poller` 为什么要分层？
- 为什么 `Poller` 用抽象基类？

### Stage 2

目标：

- 写监听 socket
- 接收新连接
- 把监听 fd 纳入 `EventLoop`

### Stage 3

目标：

- 写 `TcpConnection`
- 跑通 EchoServer
- 理解输入输出缓冲区

### Stage 4

目标：

- 支持跨线程投递任务
- 支持 one loop per thread
- 做主从 Reactor

## 学习方法

每推进一个阶段，都做 3 件事：

1. 先画关系图
2. 再写最小可运行版本
3. 最后再补设计原因

你真正要追求的是“自己能讲出来”，而不是“代码长得像 muduo”。
