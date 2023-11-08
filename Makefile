.PHONY: $(MAKECMDGOALS)

compile-shaders:
	glslc res/shaders/shader.vert -o res/shaders/vert.spv
	glslc res/shaders/shader.frag -o res/shaders/frag.spv

format:
	find src -type f -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format -i
