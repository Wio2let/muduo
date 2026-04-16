#include "muduo/CurrentThread.h"
#include "muduo/Timestamp.h"

#include <iostream>

int main() {
    muduo::Timestamp now = muduo::Timestamp::now();
    std::cout << "muduo stage0 bootstrap ok\n";
    std::cout << "thread id:" << muduo::CurrentThread::tid() << '\n';
    std::cout << "now(us):" << now.toString() << '\n';

    return 0;
}
