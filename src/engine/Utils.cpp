#include "Utils.hpp"

#include <spdlog/spdlog.h>

#include <sstream>

namespace engine {
std::string Utils::versionNumberToString(uint32_t versionNumber) {
  std::stringstream versionString;

  versionString << VK_VERSION_MAJOR(versionNumber) << "."
                << VK_VERSION_MINOR(versionNumber) << "."
                << VK_VERSION_PATCH(versionNumber);

  return versionString.str();
}

void Utils::printPhysicalDeviceInfo(const vk::PhysicalDevice &physicalDevice) {
  vk::PhysicalDeviceProperties deviceProps = physicalDevice.getProperties();

  const char msg[] =
      R"(Picked device:
           Name: {}
           API version: v{}
           Driver version: v{})";

  SPDLOG_DEBUG(
      msg,
      deviceProps.deviceName,
      versionNumberToString(deviceProps.apiVersion),
      versionNumberToString(deviceProps.driverVersion)
  );
}
}  // namespace engine
