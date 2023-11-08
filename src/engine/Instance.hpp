#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"

namespace engine {

class Instance {
 public:
  explicit Instance(const VkInstanceCreateInfo& createInfo,
                    const VkAllocationCallbacks* alloc = nullptr);

  virtual ~Instance();

  [[nodiscard]] VkInstance getHandle() const;

 private:
  VkInstance m_instance = nullptr;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // INSTANCE_HPP
