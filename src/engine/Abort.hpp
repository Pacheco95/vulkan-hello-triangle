#ifndef ABORT_HPP
#define ABORT_HPP

#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>

#include <stdexcept>

#define ABORT_ON_FAIL(result, _fmt, ...)                        \
  if ((result) != VkResult::VK_SUCCESS) {                       \
    const char* errorDetail = string_VkResult(result);          \
    SPDLOG_CRITICAL(errorDetail);                               \
    throw std::runtime_error(fmt::format(_fmt, ##__VA_ARGS__)); \
  }                                                             \
  0

#define ABORT(...)              \
  SPDLOG_CRITICAL(__VA_ARGS__); \
  throw std::runtime_error(fmt::format(__VA_ARGS__))

#endif  // ABORT_HPP
