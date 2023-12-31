#include "ModelLoader.hpp"

#include <tiny_obj_loader.h>

#include "Abort.hpp"

namespace engine {
void ModelLoader::loadObj(
    const std::string& modelPath,
    std::vector<Vertex>& vertices,
    std::vector<uint32_t>& indices
) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(
          &attrib, &shapes, &materials, &warn, &err, modelPath.c_str()
      )) {
    ABORT(warn + err);
  }

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex{};

      vertex.pos = {
          attrib.vertices[3 * index.vertex_index + 0],
          attrib.vertices[3 * index.vertex_index + 1],
          attrib.vertices[3 * index.vertex_index + 2]};

      vertex.texCoord = {
          attrib.texcoords[2 * index.texcoord_index + 0],
          1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.emplace_back(vertex);
      }

      indices.emplace_back(uniqueVertices[vertex]);
    }
  }
}
}  // namespace engine
