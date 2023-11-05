#ifndef ABORT_HPP
#define ABORT_HPP

#include <spdlog/spdlog.h>

#include <stdexcept>

#define ABORT(msg)      \
  SPDLOG_CRITICAL(msg); \
  throw std::runtime_error(msg)

#endif  // ABORT_HPP
