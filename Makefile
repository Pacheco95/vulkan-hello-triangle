.PHONY: $(MAKECMDGOALS)

clean:
	rm -rf cmake-build*

compile-shaders:
	glslc res/shaders/shader.vert -o res/shaders/vert.spv
	glslc res/shaders/shader.frag -o res/shaders/frag.spv

format:
	find src res -type f \
		-name "CMakeLists.txt" -o \
		-name "*.cpp" -o \
		-name "*.hpp" -o \
		-name "*.h" -o \
		-name "*.vert" -o \
		-name "*.frag" | xargs clang-format -i
