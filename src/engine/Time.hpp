#ifndef TIME_HPP
#define TIME_HPP

#include <chrono>

namespace engine {
using namespace std::chrono;

class Time {
 public:
  template <typename Precision = float>
  Precision deltaTime();

 private:
  time_point<high_resolution_clock> lastTime = high_resolution_clock::now();
};

#include "Time.inl"
}  // namespace engine

#endif  // TIME_HPP
