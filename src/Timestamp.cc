#include "muduo/Timestamp.h"

#include <chrono>

namespace muduo {

Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
    : microSecondsSinceEpoch_(microSecondsSinceEpoch) {}
// 成员变量加_
Timestamp Timestamp::now() {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(now).count();

    return Timestamp(microSeconds);
}

int64_t Timestamp::microSecondsSinceEpoch() const {
    return microSecondsSinceEpoch_;
}

std::string Timestamp::toString() const {
    return std::to_string(microSecondsSinceEpoch_);
}

}
