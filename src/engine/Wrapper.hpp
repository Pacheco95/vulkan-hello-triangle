#ifndef WRAPPER_HPP
#define WRAPPER_HPP

#include <vulkan/vulkan.h>

#include <Abort.hpp>
#include <regex>

template <
    typename T,
    typename CreateInfo,
    VKAPI_ATTR VkResult VKAPI_CALL (*VkCreate
    )(VkDevice device,
      const CreateInfo* pCreateInfo,
      const VkAllocationCallbacks* pAllocator,
      T* dest),
    VKAPI_ATTR void VKAPI_CALL (*VkDestroy
    )(VkDevice device, T src, const VkAllocationCallbacks* pAllocator)>
class VkWrapper {
 public:
  explicit VkWrapper(
      VkDevice device,
      const CreateInfo& createInfo,
      const VkAllocationCallbacks* allocator = nullptr
  )
      : m_device(device),
        m_allocator(allocator) {
    ABORT_ON_FAIL(
        VkCreate(m_device, &createInfo, allocator, &m_handle),
        "Failed to create {}",
        typeid(T).name()
    );

    SPDLOG_DEBUG("Created {}: {}", typeid(T).name(), fmt::ptr(m_handle));
  }

  virtual ~VkWrapper() {
    VkDestroy(m_device, m_handle, m_allocator);
    SPDLOG_DEBUG("Destroyed {}: {}", typeid(T).name(), fmt::ptr(m_handle));
  }

  operator T() { return m_handle; }

  operator T() const { return m_handle; }

 private:
  VkDevice m_device;
  const VkAllocationCallbacks* m_allocator;
  T m_handle;
};

#endif  // WRAPPER_HPP
