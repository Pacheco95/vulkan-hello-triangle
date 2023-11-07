#include "Instance.hpp"

namespace engine {
Instance::Instance(const VkInstanceCreateInfo& createInfo,
                   const VkAllocationCallbacks* alloc) {
  ABORT_ON_FAIL(vkCreateInstance(&createInfo, alloc, &m_instance),
                "Failed to create {}", typeid(Instance).name());
}

Instance::~Instance() { vkDestroyInstance(m_instance, nullptr); }

VkInstance Instance::getHandle() const { return m_instance; }
}  // namespace engine
