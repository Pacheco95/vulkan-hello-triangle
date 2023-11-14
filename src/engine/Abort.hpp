#ifndef ABORT_HPP
#define ABORT_HPP

#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>

#include <stdexcept>
#include <vulkan/vulkan.hpp>

template <typename T, typename... Args>
void ABORT_ON_FAIL(T result, const char* fmt, Args&&... args) {
  vk::resultCheck(result, fmt::format(fmt, args...).c_str());
}

template <typename... Args>
void ABORT_ON_FAIL(VkResult result, const char* fmt, Args&&... args) {
  ABORT_ON_FAIL(static_cast<vk::Result>(result), fmt, args...);
}

#define ABORT(...)              \
  SPDLOG_CRITICAL(__VA_ARGS__); \
  throw std::runtime_error(fmt::format(__VA_ARGS__))

#endif  // ABORT_HPP
