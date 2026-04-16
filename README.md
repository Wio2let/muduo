# muduo

一个按模块逐步实现的精简版 muduo 网络库练习工程。

当前实现顺序如下：

1. `base` 基础组件：`noncopyable`、`Timestamp`、`CurrentThread`
2. 单线程 Reactor：`Channel`、`Poller`、`EPollPoller`、`EventLoop`
3. TCP 接入：`Socket`、`InetAddress`、`Acceptor`
4. 连接抽象：`Buffer`、`Callbacks`、`TcpConnection`
5. 服务器封装：`TcpServer`
6. 多线程扩展：`EventLoopThread`、`EventLoopThreadPool`

## 当前进度

当前已经完成到第 2 阶段：

- 搭好构建系统
- 放入最基础的公共组件
- 用一个很小的示例确认工程可编译、可运行
- 实现最小单线程 Reactor：`EventLoop`、`Channel`、`Poller`、`EPollPoller`
- 用 `eventfd` 示例验证 `epoll_wait -> Channel -> callback` 链路
- 加入 `InetAddress`、`Socket`、`Acceptor`
- 用监听示例验证 `listen fd -> epoll -> accept4()` 链路

## 说明

先从基础层开始有两个目的：

- 后面所有模块都会依赖这些公共组件
- 先把工程骨架、命名和头文件组织方式固定下来

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
./build/stage2_acceptor
```

## 下一步

下一阶段进入连接抽象层，开始补充：

- `Buffer`
- `Callbacks`
- `TcpConnection`

完成后，新接入的 `connfd` 就能被包装成连接对象，而不是只停留在 `accept4()` 返回的裸 fd。
