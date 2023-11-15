#include <stb_image.h>

#include <algorithm>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <set>
#include <sstream>
#include <vulkan/vulkan.hpp>

#include "BinaryLoader.hpp"
#include "Camera.hpp"
#include "Config.hpp"
#include "ModelLoader.hpp"
#include "QueueFamily.hpp"
#include "Time.hpp"
#include "Utils.hpp"
#include "ValidationLayer.hpp"
#include "Vertex.hpp"
#include "Window.hpp"

namespace app {
using namespace engine;

constexpr char MODEL_PATH[] = "res/models/viking_room.obj";
constexpr char TEXTURE_PATH[] = "res/textures/viking_room.png";

struct SwapChainSupportDetails {
  SwapChainSupportDetails(
      const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface
  )
      : capabilities(device.getSurfaceCapabilitiesKHR(surface)),
        formats(device.getSurfaceFormatsKHR(surface)),
        presentModes(device.getSurfacePresentModesKHR(surface)) {}

  const vk::SurfaceCapabilitiesKHR capabilities;
  const std::vector<vk::SurfaceFormatKHR> formats;
  const std::vector<vk::PresentModeKHR> presentModes;
};

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

class Application {
 public:
  void run() {
    initWindow();
    initVulkan();
    initCamera();
    mainLoop();
    cleanup();

    SPDLOG_ERROR(
        "{} validation errors found", ValidationLayer::getErrorsCount()
    );
  }

 private:
  std::unique_ptr<Camera> m_camera;
  std::unique_ptr<Window> m_window;

  vk::Instance m_instance;
  std::unique_ptr<ValidationLayer> m_validationLayer;

  vk::PhysicalDevice m_physicalDevice;

  vk::Device m_device;
  vk::Queue m_graphicsQueue;
  vk::Queue m_presentQueue;

  vk::SwapchainKHR m_swapChain;
  std::vector<vk::Image> m_swapChainImages;
  std::vector<vk::ImageView> m_swapChainImageViews;

  vk::Format m_swapChainImageFormat;
  vk::Extent2D m_swapChainExtent;

  vk::SurfaceKHR m_surface;

  vk::RenderPass m_renderPass;
  vk::DescriptorPool m_descriptorPool;
  std::vector<vk::DescriptorSet> m_descriptorSets;
  vk::DescriptorSetLayout m_descriptorSetLayout;
  vk::PipelineLayout m_pipelineLayout;
  vk::Pipeline m_graphicsPipeline;

  std::vector<vk::Framebuffer> m_swapChainFrameBuffers;
  vk::CommandPool m_commandPool;
  std::vector<vk::CommandBuffer> m_commandBuffers;

  std::vector<vk::Semaphore> m_imageAvailableSemaphores;
  std::vector<vk::Semaphore> m_renderFinishedSemaphores;
  std::vector<vk::Fence> m_inFlightFences;

  bool m_framebufferResized = false;

  uint32_t m_currentFrame = 0;

  std::vector<Vertex> m_vertices;
  std::vector<uint32_t> m_indices;
  vk::Buffer m_vertexBuffer;
  vk::DeviceMemory m_vertexBufferMemory;

  vk::Buffer m_indexBuffer;
  vk::DeviceMemory m_indexBufferMemory;

  std::vector<vk::Buffer> m_uniformBuffers;
  std::vector<vk::DeviceMemory> m_uniformBuffersMemory;
  std::vector<void*> m_uniformBuffersMapped;

  uint32_t m_mipLevels;
  vk::Image m_textureImage;
  vk::DeviceMemory m_textureImageMemory;

  vk::ImageView m_textureImageView;
  vk::Sampler m_textureSampler;

  vk::Image m_depthImage;
  vk::DeviceMemory m_depthImageMemory;
  vk::ImageView m_depthImageView;

  vk::SampleCountFlagBits m_msaaSamples = vk::SampleCountFlagBits::e1;

  vk::Image m_colorImage;
  vk::DeviceMemory m_colorImageMemory;
  vk::ImageView m_colorImageView;

  void initWindow() {
    m_window = std::make_unique<Window>(
        Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::WINDOW_TITLE
    );

    glfwSetWindowUserPointer(*m_window, this);
    glfwSetFramebufferSizeCallback(*m_window, framebufferResizeCallback);
    glfwSetMouseButtonCallback(*m_window, mouseCallback);
  }

  void initVulkan() {
    createInstance();

    if (Config::IS_VALIDATION_LAYERS_ENABLED) {
      setupDebugMessenger();
    }

    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createColorResources();
    createDepthResources();
    createFrameBuffers();
    createCommandPool();
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    loadModel();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
  }

  void initCamera() { m_camera = std::make_unique<Camera>(*m_window); }

  void mainLoop() {
    Time time;

    while (m_window->isOpen()) {
      m_window->pollEvents();
      drawFrame();
      m_camera->update(time.deltaTime());
      m_currentFrame = (m_currentFrame + 1) % Config::MAX_FRAMES_IN_FLIGHT;
    }

    m_device.waitIdle();
  }

  void cleanup() {
    cleanupSwapChain();

    m_device.destroy(m_textureSampler);
    m_device.destroy(m_textureImageView);

    m_device.destroy(m_textureImage);
    m_device.free(m_textureImageMemory);

    m_uniformBuffers.clear();
    m_uniformBuffersMemory.clear();

    m_device.destroy(m_descriptorPool);
    m_device.destroy(m_descriptorSetLayout);

    m_device.destroy(m_graphicsPipeline);
    m_device.destroy(m_pipelineLayout);
    m_device.destroyRenderPass(m_renderPass);

    m_device.destroy(m_indexBuffer);
    m_device.free(m_indexBufferMemory);

    m_device.destroy(m_vertexBuffer);
    m_device.free(m_vertexBufferMemory);

    m_renderFinishedSemaphores.clear();
    m_imageAvailableSemaphores.clear();
    m_inFlightFences.clear();

    m_device.destroy(m_commandPool);

    m_device.destroy();
    m_instance.destroySurfaceKHR(m_surface);

    m_validationLayer.reset();
    m_instance.destroy();

    m_camera.reset();
    m_window.reset();
  }

  void createInstance() {
    auto appInfo =
        vk::ApplicationInfo()
            .setPApplicationName(Config::APP_NAME)
            .setApplicationVersion(Config::APP_VERSION)
            .setPEngineName("No Engine")
            .setEngineVersion(VK_MAKE_API_VERSION(0, 1, 0, 0))
            .setApiVersion(VK_API_VERSION_1_0);


    const auto& extensionNames = Window::getRequiredExtensions();

    vk::InstanceCreateInfo createInfo(
        {}, &appInfo, Config::VALIDATION_LAYERS, extensionNames
    );

    m_instance = vk::createInstance(createInfo);
  }

  void setupDebugMessenger() {
    m_validationLayer = std::make_unique<ValidationLayer>(m_instance);

    std::vector<vk::LayerProperties> instanceLayerProperties =
        vk::enumerateInstanceLayerProperties();

    if (!m_validationLayer->checkLayers(
            Config::VALIDATION_LAYERS, instanceLayerProperties
        )) {
      ABORT("Validation layers requested, but not available");
    }
  }

  void createSurface() {
    VkSurfaceKHR surface;
    VkResult createSurfaceResult =
        glfwCreateWindowSurface(m_instance, *m_window, nullptr, &surface);

    ABORT_ON_FAIL(
        vk::Result(createSurfaceResult), "Failed to create window surface"
    );
    m_surface = vk::SurfaceKHR(surface);
  }

  void pickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> devices =
        m_instance.enumeratePhysicalDevices();

    for (const auto& device : devices) {
      if (isDeviceSuitable(device, m_surface)) {
        m_physicalDevice = device;
        m_msaaSamples = getMaxUsableSampleCount();
        break;
      }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
      ABORT("Failed to find a suitable GPU");
    }

    SPDLOG_DEBUG("Using MSAAx{}", static_cast<uint8_t>(m_msaaSamples));

    Utils::printPhysicalDeviceInfo(m_physicalDevice);
  }

  static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR>& availableFormats
  ) {
    for (const auto& availableFormat : availableFormats) {
      if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
          availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
        return availableFormat;
      }
    }

    return availableFormats.front();
  }

  static vk::PresentModeKHR chooseSwapPresentMode(
      const std::vector<vk::PresentModeKHR>& availablePresentModes
  ) {
    for (const auto& availablePresentMode : availablePresentModes) {
      if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
        return availablePresentMode;
      }
    }

    return vk::PresentModeKHR::eFifo;
  }

  vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities
  ) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(*m_window, &width, &height);

    vk::Extent2D actualExtent = {
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
    QueueFamilyIndices familyIndices =
        QueueFamily::findIndices(m_physicalDevice, m_surface);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilies = {
        familyIndices.graphicsFamily.value(),
        familyIndices.presentFamily.value()};

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {
      vk::DeviceQueueCreateInfo queueCreateInfo{};

      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.emplace_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;


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

    m_device = m_physicalDevice.createDevice(deviceCreateInfo);

    m_graphicsQueue =
        m_device.getQueue(familyIndices.graphicsFamily.value(), 0);
    m_presentQueue = m_device.getQueue(familyIndices.presentFamily.value(), 0);
  }

  static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device) {
    const auto& availableExtensions =
        device.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(
        Config::DEVICE_EXTENSIONS.begin(), Config::DEVICE_EXTENSIONS.end()
    );

    for (const auto& extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }

  static bool isDeviceSuitable(
      const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface
  ) {
    QueueFamilyIndices indices = QueueFamily::findIndices(device, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate;

    if (extensionsSupported) {
      SwapChainSupportDetails swapChainSupport(device, surface);

      swapChainAdequate = !swapChainSupport.formats.empty() &&
                          !swapChainSupport.presentModes.empty();
    }

    vk::PhysicalDeviceFeatures supportedFeatures;
    device.getFeatures(&supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
           supportedFeatures.samplerAnisotropy;
  }

  void createSwapChain() {
    SwapChainSupportDetails swapChainSupport(m_physicalDevice, m_surface);

    vk::SurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);

    vk::PresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);

    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    auto createInfo =
        vk::SwapchainCreateInfoKHR()
            .setSurface(m_surface)
            .setMinImageCount(imageCount)
            .setImageFormat(surfaceFormat.format)
            .setImageColorSpace(surfaceFormat.colorSpace)
            .setImageExtent(extent)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices familyIndices =
        QueueFamily::findIndices(m_physicalDevice, m_surface);

    uint32_t queueFamilyIndices[] = {
        familyIndices.graphicsFamily.value(),
        familyIndices.presentFamily.value()};

    if (familyIndices.graphicsFamily != familyIndices.presentFamily) {
      createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    m_swapChain = m_device.createSwapchainKHR(createInfo);

    m_swapChainImages = m_device.getSwapchainImagesKHR(m_swapChain);

    SPDLOG_DEBUG("Got {} swap chain images", m_swapChainImages.size());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
  }

  void createImageViews() {
    m_swapChainImageViews.reserve(m_swapChainImages.size());

    for (auto& swapChainImage : m_swapChainImages) {
      vk::ImageView view = createImageView(
          swapChainImage,
          m_swapChainImageFormat,
          vk::ImageAspectFlagBits::eColor,
          1
      );
      m_swapChainImageViews.emplace_back(std::move(view));
    }
  }

  vk::ShaderModule createShaderModule(const std::vector<char>& code) {
    vk::ShaderModuleCreateInfo createInfo{};

    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    vk::ShaderModule module = m_device.createShaderModule(createInfo);
    return module;
  }

  void createRenderPass() {
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapChainImageFormat;
    colorAttachment.samples = m_msaaSamples;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = m_msaaSamples;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout =
        vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = m_swapChainImageFormat;
    colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subPass{};
    subPass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subPass.colorAttachmentCount = 1;
    subPass.pColorAttachments = &colorAttachmentRef;
    subPass.pDepthStencilAttachment = &depthAttachmentRef;
    subPass.pResolveAttachments = &colorAttachmentResolveRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput |
        vk::PipelineStageFlagBits::eLateFragmentTests;
    dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite |
                               vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    dependency.dstStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput |
        vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite |
                               vk::AccessFlagBits::eDepthStencilAttachmentWrite;

    std::array<vk::AttachmentDescription, 3> attachments = {
        colorAttachment, depthAttachment, colorAttachmentResolve};

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subPass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    m_renderPass = m_device.createRenderPass(renderPassInfo);
  }

  void createGraphicsPipeline() {
    auto vertShaderCode = BinaryLoader::load("res/shaders/shader.vert.spv");
    auto fragShaderCode = BinaryLoader::load("res/shaders/shader.frag.spv");

    vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};

    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo, fragShaderStageInfo};

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizer.depthBiasEnable = VK_FALSE;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = m_msaaSamples;
    multisampling.sampleShadingEnable = VK_TRUE;
    multisampling.minSampleShading = .2f;

    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = vk::CompareOp::eLess;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount =
        static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};

    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

    m_pipelineLayout = m_device.createPipelineLayout(pipelineLayoutInfo);

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    vk::Result result;
    std::tie(result, m_graphicsPipeline) =
        m_device.createGraphicsPipeline(nullptr, pipelineInfo);
  }

  void createFrameBuffers() {
    m_swapChainFrameBuffers.reserve(m_swapChainImageViews.size());

    for (const auto& imageView : m_swapChainImageViews) {
      std::array<vk::ImageView, 3> attachments = {
          m_colorImageView, m_depthImageView, imageView};

      vk::FramebufferCreateInfo framebufferInfo{};

      framebufferInfo.renderPass = m_renderPass;
      framebufferInfo.attachmentCount =
          static_cast<uint32_t>(attachments.size());
      framebufferInfo.pAttachments = attachments.data();
      framebufferInfo.width = m_swapChainExtent.width;
      framebufferInfo.height = m_swapChainExtent.height;
      framebufferInfo.layers = 1;

      m_swapChainFrameBuffers.emplace_back(
          m_device.createFramebuffer(framebufferInfo)
      );
    }
  }

  void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices =
        QueueFamily::findIndices(m_physicalDevice, m_surface);

    vk::CommandPoolCreateInfo poolInfo{};

    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    m_commandPool = m_device.createCommandPool(poolInfo);
  }

  void copyBufferToImage(
      vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height
  ) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::BufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = vk::Offset3D{0, 0, 0};
    region.imageExtent = vk::Extent3D{width, height, 1};


    commandBuffer.copyBufferToImage(
        buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region
    );

    endSingleTimeCommands(commandBuffer);
  }

  void createColorResources() {
    vk::Format colorFormat = m_swapChainImageFormat;

    createImage(
        m_swapChainExtent.width,
        m_swapChainExtent.height,
        1,
        m_msaaSamples,
        colorFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransientAttachment |
            vk::ImageUsageFlagBits::eColorAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        m_colorImage,
        m_colorImageMemory
    );

    m_colorImageView = vk::ImageView(createImageView(
        m_colorImage, colorFormat, vk::ImageAspectFlagBits::eColor, 1
    ));
  }

  void createDepthResources() {
    vk::Format depthFormat = findDepthFormat();

    createImage(
        m_swapChainExtent.width,
        m_swapChainExtent.height,
        1,
        m_msaaSamples,
        depthFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        m_depthImage,
        m_depthImageMemory
    );

    m_depthImageView = vk::ImageView(createImageView(
        m_depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1
    ));
  }

  vk::Format findSupportedFormat(
      const std::vector<vk::Format>& candidates,
      vk::ImageTiling tiling,
      vk::FormatFeatureFlags features
  ) {
    for (const vk::Format& format : candidates) {
      vk::FormatProperties props = m_physicalDevice.getFormatProperties(format);

      if (tiling == vk::ImageTiling::eLinear &&
          (props.linearTilingFeatures & features) == features) {
        return format;
      }

      if (tiling == vk::ImageTiling::eOptimal &&
          (props.optimalTilingFeatures & features) == features) {
        return format;
      }
    }

    ABORT("Failed to find supported format");
  }

  vk::Format findDepthFormat() {
    std::vector<vk::Format> candidates{
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint};

    vk::Format optimalDepthFormat = findSupportedFormat(
        candidates,
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );

    return optimalDepthFormat;
  }

  void createTextureImage() {
    using std::floor;
    using std::log2;
    using std::max;

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(
        TEXTURE_PATH, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha
    );
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    m_mipLevels =
        static_cast<uint32_t>(floor(log2(max(texWidth, texHeight)))) + 1;

    if (!pixels) {
      ABORT("Failed to load texture image");
    }

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(
        imageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vk::Result ignored =
        m_device.mapMemory(stagingBufferMemory, 0, imageSize, {}, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    m_device.unmapMemory(stagingBufferMemory);

    stbi_image_free(pixels);

    createImage(
        texWidth,
        texHeight,
        m_mipLevels,
        vk::SampleCountFlagBits::e1,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferSrc |
            vk::ImageUsageFlagBits::eTransferDst |
            vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        m_textureImage,
        m_textureImageMemory
    );

    transitionImageLayout(
        m_textureImage,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        m_mipLevels
    );

    copyBufferToImage(
        stagingBuffer,
        m_textureImage,
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight)
    );

    transitionImageLayout(
        m_textureImage,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        m_mipLevels
    );

    copyBufferToImage(
        stagingBuffer,
        m_textureImage,
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight)
    );

    generateMipmaps(
        m_textureImage,
        vk::Format::eR8G8B8A8Srgb,
        texWidth,
        texHeight,
        m_mipLevels
    );
  }

  void generateMipmaps(
      vk::Image image,
      vk::Format imageFormat,
      int32_t texWidth,
      int32_t texHeight,
      uint32_t mipLevels
  ) {
    // Check if image format supports linear blitting
    vk::FormatProperties formatProperties;
    m_physicalDevice.getFormatProperties(imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures &
          vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
      throw std::runtime_error(
          "texture image format does not support linear blitting!"
      );
    }

    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier{};
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
      barrier.subresourceRange.baseMipLevel = i - 1;
      barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
      barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;


      commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTransfer,
          vk::PipelineStageFlagBits::eTransfer,
          {},
          0,
          nullptr,
          0,
          nullptr,
          1,
          &barrier
      );

      vk::ImageBlit blit{};
      blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
      blit.srcOffsets[1] = vk::Offset3D{mipWidth, mipHeight, 1};
      blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      blit.srcSubresource.mipLevel = i - 1;
      blit.srcSubresource.baseArrayLayer = 0;
      blit.srcSubresource.layerCount = 1;
      blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
      blit.dstOffsets[1] = vk::Offset3D{
          mipWidth > 1 ? mipWidth / 2 : 1,
          mipHeight > 1 ? mipHeight / 2 : 1,
          1};
      blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      blit.dstSubresource.mipLevel = i;
      blit.dstSubresource.baseArrayLayer = 0;
      blit.dstSubresource.layerCount = 1;

      commandBuffer.blitImage(
          image,
          vk::ImageLayout::eTransferSrcOptimal,
          image,
          vk::ImageLayout::eTransferDstOptimal,
          1,
          &blit,
          vk::Filter::eLinear
      );

      barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

      commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTransfer,
          vk::PipelineStageFlagBits::eFragmentShader,
          {},
          0,
          nullptr,
          0,
          nullptr,
          1,
          &barrier
      );

      if (mipWidth > 1) mipWidth /= 2;
      if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader,
        {},
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

    endSingleTimeCommands(commandBuffer);
  }
  void createImage(
      uint32_t width,
      uint32_t height,
      uint32_t mipLevels,
      vk::SampleCountFlagBits numSamples,
      vk::Format format,
      vk::ImageTiling tiling,
      vk::ImageUsageFlags usage,
      vk::MemoryPropertyFlags properties,
      vk::Image& image,
      vk::DeviceMemory& imageMemory
  ) {
    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    image = m_device.createImage(imageInfo);

    vk::MemoryRequirements memRequirements;
    m_device.getImageMemoryRequirements(image, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits, properties);

    imageMemory = m_device.allocateMemory(allocInfo);

    m_device.bindImageMemory(image, imageMemory, 0);
  }

  void createTextureImageView() {
    m_textureImageView = vk::ImageView(createImageView(
        m_textureImage,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor,
        m_mipLevels
    ));
  }

  vk::ImageView createImageView(
      vk::Image image,
      vk::Format format,
      vk::ImageAspectFlags aspectFlags,
      uint32_t mipLevels
  ) {
    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.image = image;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vk::ImageView imageView = m_device.createImageView(viewInfo);

    return imageView;
  }

  void createTextureSampler() {
    vk::PhysicalDeviceProperties properties = m_physicalDevice.getProperties();

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
    samplerInfo.mipLodBias = 0.0f;

    m_textureSampler = m_device.createSampler(samplerInfo);
  }

  void loadModel() { ModelLoader::loadObj(MODEL_PATH, m_vertices, m_indices); }

  void createVertexBuffer() {
    vk::DeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vk::Result ignored =
        m_device.mapMemory(stagingBufferMemory, {}, bufferSize, {}, &data);
    memcpy(data, m_vertices.data(), (size_t)bufferSize);
    m_device.unmapMemory(stagingBufferMemory);

    createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst |
            vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        m_vertexBuffer,
        m_vertexBufferMemory
    );

    copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);
  }

  void createIndexBuffer() {
    vk::DeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vk::Result ignored =
        m_device.mapMemory(stagingBufferMemory, {}, bufferSize, {}, &data);
    memcpy(data, m_indices.data(), (size_t)bufferSize);
    m_device.unmapMemory(stagingBufferMemory);

    createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst |
            vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        m_indexBuffer,
        m_indexBufferMemory
    );

    copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);
  }

  void createUniformBuffers() {
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    m_uniformBuffers.resize(Config::MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMemory.resize(Config::MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMapped.resize(Config::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++) {
      createBuffer(
          bufferSize,
          vk::BufferUsageFlagBits::eUniformBuffer,
          vk::MemoryPropertyFlagBits::eHostVisible |
              vk::MemoryPropertyFlagBits::eHostCoherent,
          m_uniformBuffers[i],
          m_uniformBuffersMemory[i]
      );


      abortOnFail(
          m_device.mapMemory(
              m_uniformBuffersMemory[i],
              {},
              bufferSize,
              {},
              &m_uniformBuffersMapped[i]
          ),
          "Failed to map memory"
      );
    }
  }

  void createDescriptorPool() {
    auto MAX_FRAMES_IN_FLIGHT =
        static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT);

    std::array<vk::DescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
    poolSizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;

    poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
    poolSizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;


    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

    m_descriptorPool = m_device.createDescriptorPool(poolInfo);
  }

  void createDescriptorSets() {
    std::vector<vk::DescriptorSetLayout> layouts(
        Config::MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout
    );
    vk::DescriptorSetAllocateInfo allocInfo{};

    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount =
        static_cast<uint32_t>(Config::MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets = m_device.allocateDescriptorSets(allocInfo);

    if (m_descriptorSets.size() != allocInfo.descriptorSetCount) {
      ABORT("Failed to allocate descriptor sets");
    }

    for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++) {
      vk::DescriptorBufferInfo bufferInfo{};
      bufferInfo.buffer = m_uniformBuffers[i];
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(UniformBufferObject);

      vk::DescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      imageInfo.imageView = m_textureImageView;
      imageInfo.sampler = m_textureSampler;

      std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};

      descriptorWrites[0].dstSet = m_descriptorSets[i];
      descriptorWrites[0].dstBinding = 0;
      descriptorWrites[0].dstArrayElement = 0;
      descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
      descriptorWrites[0].descriptorCount = 1;
      descriptorWrites[0].pBufferInfo = &bufferInfo;

      descriptorWrites[1].dstSet = m_descriptorSets[i];
      descriptorWrites[1].dstBinding = 1;
      descriptorWrites[1].dstArrayElement = 0;
      descriptorWrites[1].descriptorType =
          vk::DescriptorType::eCombinedImageSampler;
      descriptorWrites[1].descriptorCount = 1;
      descriptorWrites[1].pImageInfo = &imageInfo;

      m_device.updateDescriptorSets(
          static_cast<uint32_t>(descriptorWrites.size()),
          descriptorWrites.data(),
          0,
          nullptr
      );
    }
  }

  void createBuffer(
      vk::DeviceSize size,
      vk::BufferUsageFlags usage,
      vk::MemoryPropertyFlags properties,
      vk::Buffer& buffer,
      vk::DeviceMemory& bufferMemory
  ) {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    buffer = m_device.createBuffer(bufferInfo);

    vk::MemoryRequirements memRequirements;
    m_device.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits, properties);

    bufferMemory = m_device.allocateMemory(allocInfo);

    m_device.bindBufferMemory(buffer, bufferMemory, 0);
  }

  void copyBuffer(
      const vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize size
  ) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::BufferCopy copyRegion{};
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
  }

  uint32_t findMemoryType(
      uint32_t typeFilter, vk::MemoryPropertyFlags properties
  ) {
    vk::PhysicalDeviceMemoryProperties memProperties =
        m_physicalDevice.getMemoryProperties();

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

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(m_commandBuffers.size());

    vk::to_string(vk::Result::eErrorOutOfDateKHR);
    abortOnFail(
        m_device.allocateCommandBuffers(&allocInfo, m_commandBuffers.data()),
        "Failed to allocate command buffers"
    );
  }

  void recordCommandBuffer(
      vk::CommandBuffer commandBuffer, uint32_t imageIndex
  ) {
    vk::CommandBufferBeginInfo beginInfo{};


    abortOnFail(
        commandBuffer.begin(&beginInfo),
        "Failed to begin recording command buffer"
    );

    vk::RenderPassBeginInfo renderPassInfo{};

    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_swapChainFrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = m_swapChainExtent;

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(
        &renderPassInfo, vk::SubpassContents::eInline
    );

    commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, m_graphicsPipeline
    );

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapChainExtent.width;
    viewport.height = (float)m_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    commandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{0, 0};
    scissor.extent = m_swapChainExtent;
    commandBuffer.setScissor(0, 1, &scissor);

    vk::Buffer vertexBuffers[] = {m_vertexBuffer};
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

    commandBuffer.bindIndexBuffer(m_indexBuffer, 0, vk::IndexType::eUint32);

    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_pipelineLayout,
        0,
        1,
        &m_descriptorSets[m_currentFrame],
        0,
        nullptr
    );

    commandBuffer.drawIndexed(
        static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0
    );

    commandBuffer.endRenderPass();

    // Todo check why vulkan-hpp returns void instead of vk::Result
    commandBuffer.end();
  }

  void createSyncObjects() {
    m_imageAvailableSemaphores.reserve(Config::MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.reserve(Config::MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.reserve(Config::MAX_FRAMES_IN_FLIGHT);

    vk::SemaphoreCreateInfo semaphoreInfo{};

    vk::FenceCreateInfo fenceInfo{};

    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    vk::Device device = m_device;

    for (auto i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; ++i) {
      m_imageAvailableSemaphores.emplace_back(
          device.createSemaphore(semaphoreInfo)
      );
      m_renderFinishedSemaphores.emplace_back(
          device.createSemaphore(semaphoreInfo)
      );
      m_inFlightFences.emplace_back(device.createFence(fenceInfo));
    }
  }

  void drawFrame() {
    vk::Fence inFlightFence = m_inFlightFences[m_currentFrame];
    vk::Semaphore imageAvailableSemaphore =
        m_imageAvailableSemaphores[m_currentFrame];
    vk::SwapchainKHR swapChain = m_swapChain;
    vk::Device device = m_device;
    vk::CommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];
    vk::Semaphore renderFinishedSemaphore =
        m_renderFinishedSemaphores[m_currentFrame];

    vk::Result result;
    result = device.waitForFences(1, &inFlightFence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    result = device.acquireNextImageKHR(
        swapChain,
        UINT64_MAX,
        imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == vk::Result::eErrorOutOfDateKHR) {
      recreateSwapChain();
      return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
      abortOnFail(result, "Failed to acquire swap chain image");
    }

    updateUniformBuffer(m_currentFrame);

    result = device.resetFences(1, &inFlightFence);

    commandBuffer.reset();
    recordCommandBuffer(commandBuffer, imageIndex);

    vk::SubmitInfo submitInfo{};


    vk::Semaphore waitSemaphores[] = {imageAvailableSemaphore};
    vk::PipelineStageFlags waitStages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk::Semaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    abortOnFail(
        m_graphicsQueue.submit(1, &submitInfo, inFlightFence),
        "Failed to submit draw command buffer"
    );

    vk::PresentInfoKHR presentInfo{};


    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = m_presentQueue.presentKHR(&presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR ||
        result == vk::Result::eSuboptimalKHR || m_framebufferResized) {
      m_framebufferResized = false;
      recreateSwapChain();
    } else if (result != vk::Result::eSuccess) {
      abortOnFail(result, "Failed to present swap chain image");
    }
  }

  void cleanupSwapChain() {
    m_device.destroy(m_colorImageView);
    m_device.destroy(m_colorImage);
    m_device.free(m_colorImageMemory);

    m_device.destroy(m_depthImageView);
    m_device.destroy(m_depthImage);
    m_device.free(m_depthImageMemory);
    m_swapChainFrameBuffers.clear();
    m_swapChainImageViews.clear();
    m_device.destroy(m_swapChain);
  }

  void recreateSwapChain() {
    int width, height;
    glfwGetFramebufferSize(*m_window, &width, &height);

    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(*m_window, &width, &height);
      glfwWaitEvents();
    }

    m_device.waitIdle();

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createColorResources();
    createDepthResources();
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

  static void mouseCallback(
      GLFWwindow* window, int button, int action, [[maybe_unused]] int mods
  ) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    bool isCameraActive =
        button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS;

    app->m_camera->setActive(isCameraActive);
    auto cursorState =
        isCameraActive ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;

    glfwSetInputMode(*app->m_window, GLFW_CURSOR, cursorState);
  }

  void createDescriptorSetLayout() {
    vk::DescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

    std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {
        uboLayoutBinding, samplerLayoutBinding};
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};

    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    m_descriptorSetLayout = m_device.createDescriptorSetLayout(layoutInfo);
  }

  void updateUniformBuffer(uint32_t currentImage) {
    float aspect =
        (float)m_swapChainExtent.width / (float)m_swapChainExtent.height;

    float fov = glm::radians(45.0f);

    UniformBufferObject ubo{};
    ubo.model = glm::mat4(1.0f);
    ubo.view = m_camera->getViewMatrix();
    ubo.proj = glm::perspective(fov, aspect, 0.1f, 10.0f);

    // Prevent image to be rendered upside down
    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
  }

  vk::CommandBuffer beginSingleTimeCommands() {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer;
    vk::Result ignored =
        m_device.allocateCommandBuffers(&allocInfo, &commandBuffer);

    vk::CommandBufferBeginInfo beginInfo{};

    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);

    return commandBuffer;
  }

  void endSingleTimeCommands(vk::CommandBuffer commandBuffer) {
    commandBuffer.end();

    vk::SubmitInfo submitInfo{};

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk::Result ignored = m_graphicsQueue.submit(1, &submitInfo, VK_NULL_HANDLE);
    m_graphicsQueue.waitIdle();

    m_device.freeCommandBuffers(m_commandPool, 1, &commandBuffer);
  }

  void transitionImageLayout(
      vk::Image image,
      vk::Format /*format*/,
      vk::ImageLayout oldLayout,
      vk::ImageLayout newLayout,
      uint32_t mipLevels
  ) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier{};
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal) {
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

      sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
      destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

      sourceStage = vk::PipelineStageFlagBits::eTransfer;
      destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
      ABORT("Unsupported layout transition");
    }

    commandBuffer.pipelineBarrier(
        sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
  }

  vk::SampleCountFlagBits getMaxUsableSampleCount() {
    static constexpr std::array samples{
        vk::SampleCountFlagBits::e64,
        vk::SampleCountFlagBits::e32,
        vk::SampleCountFlagBits::e16,
        vk::SampleCountFlagBits::e8,
        vk::SampleCountFlagBits::e4,
        vk::SampleCountFlagBits::e2,
        vk::SampleCountFlagBits::e1,
    };


    const auto& physicalDeviceProperties = m_physicalDevice.getProperties();

    vk::SampleCountFlags counts =
        physicalDeviceProperties.limits.framebufferColorSampleCounts &
        physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    for (const auto& sample : samples) {
      if (counts & sample) {
        return sample;
      }
    }

    return vk::SampleCountFlagBits::e1;
  }
};

}  // namespace app
