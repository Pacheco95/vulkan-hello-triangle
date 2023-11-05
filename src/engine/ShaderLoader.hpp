#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP

#include <string>
#include <vector>

#include "Abort.hpp"

namespace engine {

class ShaderLoader {
 public:
  typedef std::vector<char> Buffer;
  static Buffer load(const std::string& filename);
};

}  // namespace engine

#endif  // SHADER_LOADER_HPP
