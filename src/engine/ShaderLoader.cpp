#include "ShaderLoader.hpp"

#include <fstream>

namespace engine {
ShaderLoader::Buffer ShaderLoader::load(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    ABORT("Failed to open shader file {}", filename);
  }

  auto fileSize = file.tellg();
  std::vector<char> buffer(static_cast<std::vector<char>::size_type>(fileSize));

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  SPDLOG_DEBUG("Shader file loaded {} ({}B)", filename, fileSize);

  return buffer;
}
}  // namespace engine
