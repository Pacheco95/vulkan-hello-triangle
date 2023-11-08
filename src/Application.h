#include <memory>
#include <sstream>

#include "Config.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "QueueFamily.hpp"
#include "Utils.hpp"
#include "ValidationLayer.hpp"
#include "Window.hpp"

class HelloTriangleApplication {
  typedef engine::Window Window;
  typedef engine::Instance Instance;
  typedef engine::ValidationLayer ValidationLayer;
  typedef engine::Config Config;
  typedef engine::PhysicalDevice PhysicalDevice;
  typedef engine::Utils Utils;
  typedef engine::QueueFamily QueueFamily;
  typedef engine::QueueFamilyIndices QueueFamilyIndices;
  typedef engine::Device Device;

 public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

 private:
  std::unique_ptr<Window> m_window;
  std::unique_ptr<Instance> m_instance;
  std::unique_ptr<ValidationLayer> m_validationLayer;
  VkPhysicalDevice m_physicalDevice;
  std::unique_ptr<Device> m_device;
  VkQueue m_graphicsQueue;

  void initWindow() {
    m_window = std::make_unique<Window>(
        Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::WINDOW_TITLE);
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
  }

  void mainLoop() {
    while (m_window->isOpen()) {
      m_window->pollEvents();
    }
  }

  void cleanup() {
    m_device.reset();
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

    m_instance = std::make_unique<Instance>(createInfo);
  }

  void setupDebugMessenger() {
    m_validationLayer =
        std::make_unique<ValidationLayer>(m_instance->getHandle());
  }

  void pickPhysicalDevice() {
    m_physicalDevice = PhysicalDevice::pick(m_instance->getHandle());

    VkPhysicalDeviceProperties deviceProps;
    vkGetPhysicalDeviceProperties(m_physicalDevice, &deviceProps);

    Utils::printDeviceInfo(deviceProps);
  }

  void createLogicalDevice() {
    QueueFamilyIndices indices =
        QueueFamily::findSuitableQueueFamilies(m_physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;

    const std::vector<const char *> &layers = Config::VALIDATION_LAYERS;

    if (Config::IS_VALIDATION_LAYERS_ENABLED) {
      deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
      deviceCreateInfo.ppEnabledLayerNames = layers.data();
    }

    m_device =
        std::make_unique<Device>(m_physicalDevice, deviceCreateInfo, nullptr);

    m_graphicsQueue = m_device->getQueue(indices.graphicsFamily.value());
  }
};
