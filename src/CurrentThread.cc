#include "muduo/CurrentThread.h"

#include <sys/syscall.h> //调用获取线程id的函数
#include <unistd.h>      //提供POSIX的API(read,write,fork)

namespace muduo::CurrentThread {
namespace {
// thread_local关键字使得每个线程都有一份独立的cacheTid拷贝
thread_local pid_t cacheTid = 0; // 里层匿名命名空间，意味着cacheTid只能在该.cc文件中访问
}

pid_t tid() {
    //==0用来标记当前线程还未获取过id
    if (cacheTid == 0) {
        cacheTid = static_cast<pid_t>(
            ::syscall(SYS_gettid)); // static_cast是强制类型转化，因为syscall返回的是long类型;
                                    // ::表示全局作用域
    }
    return cacheTid;
}

} // namespace muduo::CurrentThread
