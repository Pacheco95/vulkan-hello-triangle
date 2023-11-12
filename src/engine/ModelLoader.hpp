#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include <string>

#include "Vertex.hpp"

namespace engine {

class ModelLoader {
 public:
  static void loadObj(
      const std::string& modelPath,
      std::vector<Vertex>& vertices,
      std::vector<uint32_t>& indices
  );
};

}  // namespace engine

#endif  // MODEL_LOADER_HPP
