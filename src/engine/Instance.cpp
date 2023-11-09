#include "Instance.hpp"

namespace engine {
Instance::Instance(const VkInstanceCreateInfo& createInfo,
                   const VkAllocationCallbacks* alloc)
    : m_alloc(alloc) {
  ABORT_ON_FAIL(vkCreateInstance(&createInfo, alloc, &m_instance),
                "Failed to create {}", typeid(Instance).name());

  SPDLOG_DEBUG("Created Instance: {}", fmt::ptr(m_instance));
}

Instance::~Instance() {
  vkDestroyInstance(m_instance, m_alloc);
  SPDLOG_DEBUG("Destroyed Instance: {}", fmt::ptr(m_instance));
}

}  // namespace engine
