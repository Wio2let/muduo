# Stage 2: TCP 接入层

这一阶段把单线程 Reactor 接到真正的 TCP 监听 socket 上。

## 模块关系

```text
         +-------------+
         | EventLoop   |
         +-------------+
                ^
                |
                v
         +-------------+
         | Channel     |
         +-------------+
                ^
                |
                v
         +-------------+        owns        +-------------+
         | Acceptor    | -----------------> | Socket      |
         +-------------+                    +-------------+
                |
                v
          new connection callback
```

`Acceptor` 不负责业务读写，它只负责监听和接收新连接。

## 建连前半段流程

```text
client connect()
        |
        v
listen fd becomes readable
        |
        v
epoll_wait returns acceptChannel
        |
        v
Channel::handleEvent()
        |
        v
Acceptor::handleRead()
        |
        v
accept4() returns connfd
        |
        v
newConnectionCallback(connfd, peerAddr)
```

后续阶段会把 `connfd` 包装成 `TcpConnection`。当前阶段只验证能够拿到这个 `connfd`。

## 三个新增类

`InetAddress`：

- 封装 `sockaddr_in`
- 提供 `toIp()`、`toIpPort()`、`toPort()`

`Socket`：

- 负责 fd 生命周期
- 封装 `bind()`、`listen()`、`accept4()`
- 设置 `SO_REUSEADDR`、`SO_REUSEPORT`

`Acceptor`：

- 拥有监听 `Socket`
- 拥有监听 fd 对应的 `Channel`
- 新连接到来时执行用户注册的回调

## 最小验收

`stage2_acceptor` 会启动一个监听端口，然后在同一个进程里启动一个临时客户端连接它：

```text
stage2_acceptor
        |
        v
listen 127.0.0.1:20080
        |
        v
client thread connects once
        |
        v
Acceptor accepts connfd
        |
        v
print peer address and quit
```

运行：

```bash
./build/stage2_acceptor
```
