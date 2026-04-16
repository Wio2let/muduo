# muduo From Scratch

这个工程不复制现成 `muduo` 代码，而是按模块一步步亲手复刻一个可运行的精简版网络库。

我们的目标不是“先写很多文件”，而是按下面的顺序逐层搭起来：

1. `base` 基础组件：`noncopyable`、`Timestamp`、`CurrentThread`
2. 单线程 Reactor：`Channel`、`Poller`、`EPollPoller`、`EventLoop`
3. TCP 接入：`Socket`、`InetAddress`、`Acceptor`
4. 连接抽象：`Buffer`、`Callbacks`、`TcpConnection`
5. 服务器封装：`TcpServer`
6. 多线程扩展：`EventLoopThread`、`EventLoopThreadPool`

## 现在做到哪里

当前已经完成到第 1 阶段：

- 搭好构建系统
- 放入最基础的公共组件
- 用一个很小的示例确认工程可编译、可运行
- 实现最小单线程 Reactor：`EventLoop`、`Channel`、`Poller`、`EPollPoller`
- 用 `eventfd` 示例验证 `epoll_wait -> Channel -> callback` 链路

## 为什么先写这几个类

`muduo` 的核心当然是 `EventLoop + epoll`，但如果一上来就写 Reactor，很容易一头扎进细节。

先写基础层有两个好处：

- 后面所有模块都会依赖这些公共组件
- 我们可以先把工程骨架、命名、头文件组织方式固定下来

## 当前目录约定

```text
muduo/
|-- CMakeLists.txt
|-- README.md
|-- docs/
|-- include/muduo/
|-- src/
`-- example/
```

## 构建

```bash
cd /home/tokisami/muduo
cmake -S . -B build
cmake --build build
./build/stage0_bootstrap
./build/stage1_eventfd
```

## 下一阶段

下一步我们会进入 TCP 接入层，开始写：

- `InetAddress`
- `Socket`
- `Acceptor`

这一步完成后，`EventLoop` 就能监听真正的 server socket，而不仅仅是 `eventfd`。
