#include "BinaryLoader.hpp"

#include <fstream>

namespace engine {
BinaryLoader::Buffer BinaryLoader::load(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    ABORT("Failed to open binary file {}", filename);
  }

  auto fileSize = file.tellg();
  std::vector<char> buffer(static_cast<std::vector<char>::size_type>(fileSize));

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  SPDLOG_DEBUG("Binary file loaded {} ({}B)", filename, fileSize);

  return buffer;
}
}  // namespace engine
