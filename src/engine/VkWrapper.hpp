#ifndef VK_WRAPPER_HPP
#define VK_WRAPPER_HPP

#include <vulkan/vulkan.h>

#include <Abort.hpp>

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
  VkWrapper(
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

  VkWrapper(VkWrapper&& o) noexcept
      : m_device(std::exchange(o.m_device, VK_NULL_HANDLE)),
        m_handle(std::exchange(o.m_handle, VK_NULL_HANDLE)),
        m_allocator(std::exchange(o.m_allocator, VK_NULL_HANDLE)){};

  virtual ~VkWrapper() {
    if (m_handle) {
      VkDestroy(m_device, m_handle, m_allocator);
      SPDLOG_DEBUG("Destroyed {}: {}", typeid(T).name(), fmt::ptr(m_handle));
    }
  }

  const T& getHandle() const { return m_handle; }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
  operator T() { return m_handle; }
  operator T() const { return m_handle; }
#pragma clang diagnostic pop

 private:
  VkDevice m_device;
  const VkAllocationCallbacks* m_allocator;
  T m_handle;
};

#endif  // VK_WRAPPER_HPP
