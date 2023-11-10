#ifndef UTILS_HPP
#define UTILS_HPP

#include <vulkan/vulkan.h>

#include <string>

namespace engine {

class Utils {
 public:
  static std::string versionNumberToString(uint32_t versionNumber);

  static void printPhysicalDeviceInfo(const VkPhysicalDevice &physicalDevice);
};

}  // namespace engine

#endif  // UTILS_HPP
