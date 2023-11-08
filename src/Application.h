#include <memory>
#include <sstream>

#include "Config.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "ValidationLayer.hpp"
#include "Window.hpp"

class HelloTriangleApplication {
  typedef engine::Window Window;
  typedef engine::Instance Instance;
  typedef engine::ValidationLayer ValidationLayer;
  typedef engine::Config Config;
  typedef engine::PhysicalDevice PhysicalDevice;

 public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

 private:
  std::shared_ptr<Window> m_window;
  std::shared_ptr<Instance> m_instance;
  std::shared_ptr<ValidationLayer> m_validationLayer;
  VkPhysicalDevice m_physicalDevice;

  void initWindow() {
    m_window = std::make_shared<Window>(
        Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::WINDOW_TITLE);
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
  }

  void mainLoop() {
    while (m_window->isOpen()) {
      m_window->pollEvents();
    }
  }

  void cleanup() {
    m_validationLayer.reset();
    m_instance.reset();
    m_window.reset();
  }

  void createInstance() {
    if (Config::IS_VALIDATION_LAYERS_ENABLED &&
        !ValidationLayer::checkValidationLayerSupport()) {
      ABORT("Validation layers requested, but not available");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = Config::APP_NAME;
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = Window::getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (Config::IS_VALIDATION_LAYERS_ENABLED) {
      const std::vector<const char *> &layers = Config::VALIDATION_LAYERS;
      createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
      createInfo.ppEnabledLayerNames = layers.data();

      ValidationLayer::populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = &debugCreateInfo;
    }

    m_instance = std::make_shared<Instance>(createInfo);
  }

  void setupDebugMessenger() {
    m_validationLayer =
        std::make_shared<ValidationLayer>(m_instance->getHandle());
  }

  void pickPhysicalDevice() {
    m_physicalDevice = PhysicalDevice::pick(m_instance->getHandle());

    VkPhysicalDeviceProperties deviceProps;
    vkGetPhysicalDeviceProperties(m_physicalDevice, &deviceProps);

    HelloTriangleApplication::printDeviceInfo(deviceProps);
  }

  static void printDeviceInfo(const VkPhysicalDeviceProperties &deviceProps) {
    const char msg[] =
        R"(Picked device:
           Name: {}
           API version: v{}
           Driver version: v{})";

    SPDLOG_DEBUG(msg, deviceProps.deviceName,
                 versionNumberToString(deviceProps.apiVersion),
                 versionNumberToString(deviceProps.driverVersion));
  }

  static std::string versionNumberToString(uint32_t versionNumber) {
    std::stringstream versionString;

    versionString << VK_VERSION_MAJOR(versionNumber) << "."
                  << VK_VERSION_MINOR(versionNumber) << "."
                  << VK_VERSION_PATCH(versionNumber);

    return versionString.str();
  }
};
