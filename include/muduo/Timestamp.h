#pragma once

#include <cstdint>
#include <string>

namespace muduo {

class Timestamp {
  public:
    Timestamp() = default;
    explicit Timestamp(int64_t microSecondsSinceEpoch);

    static Timestamp now();

    int64_t microSecondsSinceEpoch() const;
    std::string toString() const;

  private:
    int64_t microSecondsSinceEpoch_{0};
};
} // namespace muduo
