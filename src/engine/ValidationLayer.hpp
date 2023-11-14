#ifndef VALIDATION_LAYER_HPP
#define VALIDATION_LAYER_HPP

#include <vector>
#include <vulkan/vulkan.hpp>

namespace engine {

class ValidationLayer {
 public:
  explicit ValidationLayer(vk::Instance& instance);

  virtual ~ValidationLayer();

  static bool checkLayers(
      std::vector<char const*> const& layers,
      std::vector<vk::LayerProperties> const& properties
  );

 private:
  vk::Instance& m_instance;
  vk::DebugUtilsMessengerEXT debugUtilsMessenger;

  void setupDebugMessenger();
};

}  // namespace engine

#endif  // VALIDATION_LAYER_HPP
