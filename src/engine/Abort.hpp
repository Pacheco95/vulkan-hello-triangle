#pragma once

#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>

#include <stdexcept>
#include <vulkan/vulkan.hpp>

#include "Utils.hpp"

template <typename... Args>
inline void abortOnFail(
    const vk::Result &result, const std::string &fmt, Args &&...fmtArgs
) {
  vk::resultCheck(result, fmt::format(fmt, fmtArgs...).c_str());
}

template <typename... Args>
inline void abortOnFail(
    VkResult result, const std::string &fmt, Args &&...fmtArgs
) {
  abortOnFail(static_cast<vk::Result>(result), fmt, fmtArgs...);
}

template <typename... Args>
[[noreturn]] inline void abort(
    const std::string &file,
    int line,
    const std::string &function,

    const std::string &format,
    Args &&...fmtArgs
) {
  const auto &newFmt =
      engine::Utils::prefixFormatWithErrorLocation(file, line, function, format)
          .str();
  throw std::runtime_error(fmt::format(newFmt, fmtArgs...));
}

template <typename T, typename... Args>
inline void abortOnFail(
    const std::string &file,
    int line,
    const std::string &function,

    T result,
    const std::string &fmt,
    Args &&...fmtArgs
) {
  const auto &newFmt =
      engine::Utils::prefixFormatWithErrorLocation(file, line, function, fmt)
          .str();

  abortOnFail(result, newFmt, fmtArgs...);
}

#define ABORT_ON_FAIL(result, fmt, ...)                                   \
  abortOnFail(                                                            \
      __FILE__, __LINE__, __PRETTY_FUNCTION__, result, fmt, ##__VA_ARGS__ \
  )

#define ABORT(fmt, ...) \
  abort(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
