#ifndef BINARY_LOADER_HPP
#define BINARY_LOADER_HPP

#include <string>
#include <vector>

#include "Abort.hpp"

namespace engine {

class BinaryLoader {
 public:
  typedef std::vector<char> Buffer;
  static Buffer load(const std::string& filename);
};

}  // namespace engine

#endif  // BINARY_LOADER_HPP
