#ifndef UTILS_HPP
#define UTILS_HPP

#include <spdlog/formatter.h>

#include <string>
#include <vulkan/vulkan.hpp>

namespace engine {

class Utils {
 public:
  static std::string versionNumberToString(uint32_t versionNumber);

  static void printPhysicalDeviceInfo(const vk::PhysicalDevice &physicalDevice);

  static std::stringstream prefixFormatWithErrorLocation(
      const std::string &file,
      int line,
      const std::string &function,
      const std::string &fmt
  ) {
    std::string origin;
    fmt::format_to(
        std::back_inserter(origin), "{}:{} at {} | ", file, line, function
    );
    return std::stringstream() << origin << fmt;
  }
};

}  // namespace engine

#endif  // UTILS_HPP
