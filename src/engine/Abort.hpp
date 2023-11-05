#ifndef ABORT_HPP
#define ABORT_HPP

#include <spdlog/spdlog.h>

#include <stdexcept>

#define ABORT(...)              \
  SPDLOG_CRITICAL(__VA_ARGS__); \
  throw std::runtime_error(fmt::format(__VA_ARGS__))

#endif  // ABORT_HPP
