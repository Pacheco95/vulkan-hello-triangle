#include <stb_image.h>

#include <algorithm>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <set>
#include <sstream>

#include "BinaryLoader.hpp"
#include "Config.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "QueueFamily.hpp"
#include "Utils.hpp"
#include "ValidationLayer.hpp"
#include "VkWrapper.hpp"
#include "VulkanDoubleCallWrapper.hpp"
#include "VulkanWrappers.hpp"
#include "Window.hpp"

namespace app {
using namespace engine;

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats{};
  std::vector<VkPresentModeKHR> presentModes{};
};

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;

  static constexpr VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static constexpr std::array<VkVertexInputAttributeDescription, 2>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
  }
};

const std::vector<Vertex> VERTICES = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

const std::vector<uint16_t> INDICES = {0, 1, 2, 2, 3, 0};

class Application {
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
  VkQueue m_presentQueue;

  std::unique_ptr<SwapChain> m_swapChain;
  std::vector<VkImage> m_swapChainImages;
  std::vector<ImageView> m_swapChainImageViews;

  VkFormat m_swapChainImageFormat;
  VkExtent2D m_swapChainExtent;

  VkSurfaceKHR m_surface;

  std::unique_ptr<RenderPass> m_renderPass;
  std::unique_ptr<DescriptorPool> m_descriptorPool;
  std::vector<VkDescriptorSet> m_descriptorSets;
  std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout;
  std::unique_ptr<PipelineLayout> m_pipelineLayout;
  std::unique_ptr<GraphicsPipeline> m_graphicsPipeline;

  std::vector<FrameBuffer> m_swapChainFrameBuffers;
  std::unique_ptr<CommandPool> m_commandPool;
  std::vector<VkCommandBuffer> m_commandBuffers;

  std::vector<Semaphore> m_imageAvailableSemaphores;
  std::vector<Semaphore> m_renderFinishedSemaphores;
  std::vector<Fence> m_inFlightFences;

  bool m_framebufferResized = false;

  uint32_t m_currentFrame = 0;

  std::unique_ptr<Buffer> m_vertexBuffer;
  std::unique_ptr<DeviceMemory> m_vertexBufferMemory;

  std::unique_ptr<Buffer> m_indexBuffer;
  std::unique_ptr<DeviceMemory> m_indexBufferMemory;

  std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;
  std::vector<std::unique_ptr<DeviceMemory>> m_uniformBuffersMemory;
  std::vector<void*> m_uniformBuffersMapped;

  std::unique_ptr<Image> m_textureImage;
  std::unique_ptr<DeviceMemory> m_textureImageMemory;

  void initWindow() {
    m_window = std::make_unique<Window>(
        Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::WINDOW_TITLE
    );

    glfwSetWindowUserPointer(*m_window, this);
    glfwSetFramebufferSizeCallback(*m_window, framebufferResizeCallback);
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createFrameBuffers();
    createCommandPool();
    createTextureImage();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
  }

  void mainLoop() {
    while (m_window->isOpen()) {
      m_window->pollEvents();
      drawFrame();
      m_currentFrame = (m_currentFrame + 1) % Config::MAX_FRAMES_IN_FLIGHT;
    }

    vkDeviceWaitIdle(*m_device);
  }

  void cleanup() {
    cleanupSwapChain();

    m_textureImage.reset();
    m_textureImageMemory.reset();

    m_uniformBuffers.clear();
    m_uniformBuffersMemory.clear();

    m_descriptorPool.reset();
    m_descriptorSetLayout.reset();

    m_descriptorSetLayout.reset();

    m_graphicsPipeline.reset();
    m_pipelineLayout.reset();
    m_renderPass.reset();

    m_indexBuffer.reset();
    m_indexBufferMemory.reset();

    m_vertexBuffer.reset();
    m_vertexBufferMemory.reset();

    m_renderFinishedSemaphores.clear();
    m_imageAvailableSemaphores.clear();
    m_inFlightFences.clear();

    m_commandPool.reset();

    m_device.reset();
    m_validationLayer.reset();
    vkDestroySurfaceKHR(*m_instance, m_surface, nullptr);
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
      const auto& layers = Config::VALIDATION_LAYERS;
      createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
      createInfo.ppEnabledLayerNames = layers.data();

      ValidationLayer::populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = &debugCreateInfo;
    }

    m_instance = std::make_unique<Instance>(createInfo);
  }

  void setupDebugMessenger() {
    if (Config::IS_VALIDATION_LAYERS_ENABLED) {
      m_validationLayer = std::make_unique<ValidationLayer>(*m_instance);
    }
  }

  void createSurface() {
    ABORT_ON_FAIL(
        glfwCreateWindowSurface(*m_instance, *m_window, nullptr, &m_surface),
        "Failed to create window surface"
    );
  }

  void pickPhysicalDevice() {
    std::vector<VkPhysicalDevice> devices =
        PhysicalDevice::enumeratePhysicalDevices(*m_instance);

    for (const auto& device : devices) {
      if (isDeviceSuitable(device, m_surface)) {
        m_physicalDevice = device;
        break;
      }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
      ABORT("Failed to find a suitable GPU");
    }

    Utils::printPhysicalDeviceInfo(m_physicalDevice);
  }

  static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& availableFormats
  ) {
    for (const auto& availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
          availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
      }
    }

    return availableFormats.front();
  }

  static VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& availablePresentModes
  ) {
    for (const auto& availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return availablePresentMode;
      }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(*m_window, &width, &height);

    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    actualExtent.width = std::clamp(
        actualExtent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width
    );

    actualExtent.height = std::clamp(
        actualExtent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    return actualExtent;
  }

  void createLogicalDevice() {
    QueueFamilyIndices indices =
        QueueFamily::findSuitableQueueFamilies(m_physicalDevice, m_surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.emplace_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};

    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());

    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    deviceCreateInfo.enabledExtensionCount =
        static_cast<uint32_t>(Config::DEVICE_EXTENSIONS.size());

    deviceCreateInfo.ppEnabledExtensionNames = Config::DEVICE_EXTENSIONS.data();

    const auto& layers = Config::VALIDATION_LAYERS;

    if (Config::IS_VALIDATION_LAYERS_ENABLED) {
      deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
      deviceCreateInfo.ppEnabledLayerNames = layers.data();
    }

    m_device = std::make_unique<Device>(m_physicalDevice, deviceCreateInfo);

    m_graphicsQueue = m_device->getQueue(indices.graphicsFamily.value());
    m_presentQueue = m_device->getQueue(indices.presentFamily.value());
  }

  static SwapChainSupportDetails querySwapChainSupport(
      VkPhysicalDevice device, VkSurfaceKHR surface
  ) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device, surface, &details.capabilities
    );

    details.formats =
        vkCall(vkGetPhysicalDeviceSurfaceFormatsKHR, device, surface);

    details.presentModes =
        vkCall(vkGetPhysicalDeviceSurfacePresentModesKHR, device, surface);

    return details;
  }

  static bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    std::vector availableExtensions =
        vkCall(vkEnumerateDeviceExtensionProperties, device, nullptr);

    std::set<std::string> requiredExtensions(
        Config::DEVICE_EXTENSIONS.begin(), Config::DEVICE_EXTENSIONS.end()
    );

    for (const auto& extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }

  static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices =
        QueueFamily::findSuitableQueueFamilies(device, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate;

    if (extensionsSupported) {
      SwapChainSupportDetails swapChainSupport =
          querySwapChainSupport(device, surface);

      swapChainAdequate = !swapChainSupport.formats.empty() &&
                          !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
  }

  void createSwapChain() {
    SwapChainSupportDetails swapChainSupport =
        querySwapChainSupport(m_physicalDevice, m_surface);

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);

    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);

    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices =
        QueueFamily::findSuitableQueueFamilies(m_physicalDevice, m_surface);

    uint32_t queueFamilyIndices[] = {
        indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    m_swapChain = std::make_unique<SwapChain>(*m_device, createInfo);

    m_swapChainImages =
        vkCall(vkGetSwapchainImagesKHR, *m_device, *m_swapChain);

    SPDLOG_DEBUG("Got {} swap chain images", m_swapChainImages.size());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
  }

  void createImageViews() {
    m_swapChainImageViews.reserve(m_swapChainImages.size());

    for (auto& swapChainImage : m_swapChainImages) {
      VkImageViewCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      createInfo.image = swapChainImage;
      createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      createInfo.format = m_swapChainImageFormat;
      createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;

      m_swapChainImageViews.emplace_back(*m_device, createInfo);
    }
  }

  ShaderModule createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    ShaderModule module(*m_device, createInfo);
    return module;
  }

  void createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    m_renderPass = std::make_unique<RenderPass>(*m_device, renderPassInfo);
  }

  void createGraphicsPipeline() {
    auto vertShaderCode = BinaryLoader::load("res/shaders/shader.vert.spv");
    auto fragShaderCode = BinaryLoader::load("res/shaders/shader.frag.spv");

    ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount =
        static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout->getHandle();

    m_pipelineLayout =
        std::make_unique<PipelineLayout>(*m_device, pipelineLayoutInfo);

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = *m_pipelineLayout;
    pipelineInfo.renderPass = *m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    m_graphicsPipeline =
        std::make_unique<GraphicsPipeline>(*m_device, pipelineInfo);
  }

  void createFrameBuffers() {
    m_swapChainFrameBuffers.reserve(m_swapChainImageViews.size());

    for (const auto& imageView : m_swapChainImageViews) {
      VkImageView attachments[] = {imageView};

      VkFramebufferCreateInfo framebufferInfo{};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = *m_renderPass;
      framebufferInfo.attachmentCount = 1;
      framebufferInfo.pAttachments = attachments;
      framebufferInfo.width = m_swapChainExtent.width;
      framebufferInfo.height = m_swapChainExtent.height;
      framebufferInfo.layers = 1;

      m_swapChainFrameBuffers.emplace_back(*m_device, framebufferInfo);
    }
  }

  void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices =
        QueueFamily::findSuitableQueueFamilies(m_physicalDevice, m_surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    m_commandPool = std::make_unique<CommandPool>(*m_device, poolInfo);
  }

  void copyBufferToImage(
      VkBuffer buffer, VkImage image, uint32_t width, uint32_t height
  ) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    endSingleTimeCommands(commandBuffer);
  }

  void createTextureImage() {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(
        "res/textures/texture.png",
        &texWidth,
        &texHeight,
        &texChannels,
        STBI_rgb_alpha
    );
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
      ABORT("Failed to load texture image");
    }

    std::unique_ptr<Buffer> stagingBuffer;
    std::unique_ptr<DeviceMemory> stagingBufferMemory;
    createBuffer(
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(*m_device, *stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(*m_device, *stagingBufferMemory);

    stbi_image_free(pixels);

    createImage(
        texWidth,
        texHeight,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_textureImage,
        m_textureImageMemory
    );

    transitionImageLayout(
        *m_textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    copyBufferToImage(
        *stagingBuffer,
        *m_textureImage,
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight)
    );

    transitionImageLayout(
        *m_textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
  }

  void createImage(
      uint32_t width,
      uint32_t height,
      VkFormat format,
      VkImageTiling tiling,
      VkImageUsageFlags usage,
      VkMemoryPropertyFlags properties,
      std::unique_ptr<Image>& image,
      std::unique_ptr<DeviceMemory>& imageMemory
  ) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    image = std::make_unique<Image>(*m_device, imageInfo);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(*m_device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits, properties);

    imageMemory = std::make_unique<DeviceMemory>(*m_device, allocInfo);

    vkBindImageMemory(*m_device, *image, *imageMemory, 0);
  }

  void createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(VERTICES[0]) * VERTICES.size();

    std::unique_ptr<Buffer> stagingBuffer;
    std::unique_ptr<DeviceMemory> stagingBufferMemory;

    createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(*m_device, *stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, VERTICES.data(), (size_t)bufferSize);
    vkUnmapMemory(*m_device, *stagingBufferMemory);

    createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_vertexBuffer,
        m_vertexBufferMemory
    );

    copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);
  }

  void createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(INDICES[0]) * INDICES.size();

    std::unique_ptr<Buffer> stagingBuffer;
    std::unique_ptr<DeviceMemory> stagingBufferMemory;
    createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(*m_device, *stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, INDICES.data(), (size_t)bufferSize);
    vkUnmapMemory(*m_device, *stagingBufferMemory);

    createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_indexBuffer,
        m_indexBufferMemory
    );

    copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);
  }

  void createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_uniformBuffers.resize(Config::MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMemory.resize(Config::MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMapped.resize(Config::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++) {
      createBuffer(
          bufferSize,
          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
          m_uniformBuffers[i],
          m_uniformBuffersMemory[i]
      );

      vkMapMemory(
          *m_device,
          *m_uniformBuffersMemory[i],
          0,
          bufferSize,
          0,
          &m_uniformBuffersMapped[i]
      );
    }
  }

  void createDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount =
        static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT);

    m_descriptorPool = std::make_unique<DescriptorPool>(*m_device, poolInfo);
  }

  void createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(
        Config::MAX_FRAMES_IN_FLIGHT, *m_descriptorSetLayout
    );
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = *m_descriptorPool;
    allocInfo.descriptorSetCount =
        static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(Config::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(
            *m_device, &allocInfo, m_descriptorSets.data()
        ) != VK_SUCCESS) {
      ABORT("Failed to allocate descriptor sets");
    }

    for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++) {
      VkDescriptorBufferInfo bufferInfo{};
      bufferInfo.buffer = *m_uniformBuffers[i];
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(UniformBufferObject);

      VkWriteDescriptorSet descriptorWrite{};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = m_descriptorSets[i];
      descriptorWrite.dstBinding = 0;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = &bufferInfo;

      vkUpdateDescriptorSets(*m_device, 1, &descriptorWrite, 0, nullptr);
    }
  }

  void createBuffer(
      VkDeviceSize size,
      VkBufferUsageFlags usage,
      VkMemoryPropertyFlags properties,
      std::unique_ptr<Buffer>& buffer,
      std::unique_ptr<DeviceMemory>& bufferMemory
  ) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    buffer = std::make_unique<Buffer>(*m_device, bufferInfo);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*m_device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits, properties);

    bufferMemory = std::make_unique<DeviceMemory>(*m_device, allocInfo);

    vkBindBufferMemory(*m_device, *buffer, *bufferMemory, 0);
  }

  void copyBuffer(
      const std::unique_ptr<Buffer>& srcBuffer,
      std::unique_ptr<Buffer>& dstBuffer,
      VkDeviceSize size
  ) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, *srcBuffer, *dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
  }

  uint32_t findMemoryType(
      uint32_t typeFilter, VkMemoryPropertyFlags properties
  ) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
      if ((typeFilter & (1 << i)) &&
          (memProperties.memoryTypes[i].propertyFlags & properties) ==
              properties) {
        return i;
      }
    }

    ABORT("Failed to find suitable memory type");
  }

  void createCommandBuffers() {
    m_commandBuffers.resize(Config::MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = *m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(m_commandBuffers.size());

    ABORT_ON_FAIL(
        vkAllocateCommandBuffers(
            *m_device, &allocInfo, m_commandBuffers.data()
        ),
        "Failed to allocate command buffers"
    );
  }

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    ABORT_ON_FAIL(
        vkBeginCommandBuffer(commandBuffer, &beginInfo),
        "Failed to begin recording command buffer"
    );

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = *m_renderPass;
    renderPassInfo.framebuffer = m_swapChainFrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(
        commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE
    );

    vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_graphicsPipeline
    );

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapChainExtent.width;
    viewport.height = (float)m_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {*m_vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(
        commandBuffer, *m_indexBuffer, 0, VK_INDEX_TYPE_UINT16
    );

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        *m_pipelineLayout,
        0,
        1,
        &m_descriptorSets[m_currentFrame],
        0,
        nullptr
    );

    vkCmdDrawIndexed(
        commandBuffer, static_cast<uint32_t>(INDICES.size()), 1, 0, 0, 0
    );

    vkCmdEndRenderPass(commandBuffer);

    ABORT_ON_FAIL(
        vkEndCommandBuffer(commandBuffer), "Failed to record command buffer"
    );
  }

  void createSyncObjects() {
    m_imageAvailableSemaphores.reserve(Config::MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.reserve(Config::MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.reserve(Config::MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkDevice device = *m_device;

    for (auto i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; ++i) {
      m_imageAvailableSemaphores.emplace_back(device, semaphoreInfo);
      m_renderFinishedSemaphores.emplace_back(device, semaphoreInfo);
      m_inFlightFences.emplace_back(device, fenceInfo);
    }
  }

  void drawFrame() {
    VkFence inFlightFence = m_inFlightFences[m_currentFrame];
    VkSemaphore imageAvailableSemaphore =
        m_imageAvailableSemaphores[m_currentFrame];
    VkSwapchainKHR swapChain = *m_swapChain;
    VkDevice device = *m_device;
    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];
    VkSemaphore renderFinishedSemaphore =
        m_renderFinishedSemaphores[m_currentFrame];

    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        device,
        swapChain,
        UINT64_MAX,
        imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      ABORT_ON_FAIL(result, "Failed to acquire swap chain image");
    }

    updateUniformBuffer(m_currentFrame);

    vkResetFences(device, 1, &inFlightFence);

    vkResetCommandBuffer(
        commandBuffer,
        /*VkCommandBufferResetFlagBits*/ 0
    );
    recordCommandBuffer(commandBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    ABORT_ON_FAIL(
        vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, inFlightFence),
        "Failed to submit draw command buffer"
    );

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        m_framebufferResized) {
      m_framebufferResized = false;
      recreateSwapChain();
    } else if (result != VK_SUCCESS) {
      ABORT_ON_FAIL(result, "Failed to present swap chain image");
    }
  }

  void cleanupSwapChain() {
    m_swapChainFrameBuffers.clear();
    m_swapChainImageViews.clear();
    m_swapChain.reset();
  }

  void recreateSwapChain() {
    int width, height;
    glfwGetFramebufferSize(*m_window, &width, &height);

    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(*m_window, &width, &height);
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(*m_device);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createFrameBuffers();
  }

  static void framebufferResizeCallback(
      GLFWwindow* window,
      [[maybe_unused]] int width,
      [[maybe_unused]] int height
  ) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->m_framebufferResized = true;
  }

  void createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    m_descriptorSetLayout =
        std::make_unique<DescriptorSetLayout>(*m_device, layoutInfo);
  }

  void updateUniformBuffer(uint32_t currentImage) {
    using namespace std::chrono;

    static auto startTime = high_resolution_clock::now();

    auto now = high_resolution_clock::now();
    float delta = duration<float, seconds::period>(now - startTime).count();

    UniformBufferObject ubo{};

    ubo.model = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(90.0f) * delta,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    const auto& eyePosition = glm::vec3(2.0f, 2.0f, 2.0f);
    const auto& center = glm::vec3(0.0f, 0.0f, 0.0f);
    const auto& up = glm::vec3(0.0f, 0.0f, 1.0f);
    ubo.view = glm::lookAt(eyePosition, center, up);

    float aspect =
        (float)m_swapChainExtent.width / (float)m_swapChainExtent.height;

    float fov = glm::radians(45.0f);
    ubo.proj = glm::perspective(fov, aspect, 0.1f, 10.0f);

    // Prevent image to be rendered upside down
    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
  }

  VkCommandBuffer beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = *m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(*m_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
  }

  void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(*m_device, *m_commandPool, 1, &commandBuffer);
  }

  void transitionImageLayout(
      VkImage image,
      VkFormat format,
      VkImageLayout oldLayout,
      VkImageLayout newLayout
  ) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
      ABORT("Unsupported layout transition");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

    endSingleTimeCommands(commandBuffer);
  }
};

}  // namespace app
