#include "muduo/CurrentThread.h"

#include <sys/syscall.h>
#include <unistd.h>

namespace muduo::CurrentThread {

namespace {
thread_local pid_t cachedTid = 0;
}

pid_t tid() {
    if (cachedTid == 0) {
        cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
    return cachedTid;
}

}  // namespace muduo::CurrentThread
