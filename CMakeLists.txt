cmake_minimum_required(VERSION 3.26)
project(VulkanHelloTriangle)

set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(GLM_FORCE_RADIANS)
add_compile_definitions(STB_IMAGE_IMPLEMENTATION)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_WARN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
else ()
  add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
endif ()

include_directories(
        src/engine
        src/vendor
        external/spdlog/include
)

set(RESOURCES_DIR "res")
set(SHADERS_DIR "${RESOURCES_DIR}/shaders")

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(external/spdlog)

add_executable(VulkanHelloTriangle src/main.cpp
        src/engine/Config.cpp
        src/engine/Config.hpp
        src/engine/Abort.hpp
        src/Application.h
        src/engine/Instance.cpp
        src/engine/Instance.hpp
        src/engine/Window.cpp
        src/engine/Window.hpp
        src/engine/ValidationLayer.cpp
        src/engine/ValidationLayer.hpp
        src/engine/PhysicalDevice.cpp
        src/engine/PhysicalDevice.hpp
        src/engine/Utils.cpp
        src/engine/Utils.hpp
        src/engine/QueueFamily.cpp
        src/engine/QueueFamily.hpp
        src/engine/Device.cpp
        src/engine/Device.hpp
        src/engine/BinaryLoader.cpp
        src/engine/BinaryLoader.hpp
        src/engine/VkWrapper.hpp
        src/engine/VulkanDoubleCallWrapper.hpp
        src/engine/VulkanWrappers.hpp
        src/engine/Vertex.hpp
)

target_link_libraries(VulkanHelloTriangle
        PRIVATE
        glfw
        Vulkan::Vulkan
        spdlog
        dl
        pthread
        X11
        Xxf86vm
        Xrandr
        Xi
)

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${SHADERS_DIR}/*.frag"
        "${SHADERS_DIR}/*.vert"
)

foreach (GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/${SHADERS_DIR}/${FILE_NAME}.spv")
  add_custom_command(
          OUTPUT ${SPIRV}
          COMMAND glslc ${GLSL} -o ${SPIRV}
          DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach (GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(VulkanHelloTriangle Shaders)
