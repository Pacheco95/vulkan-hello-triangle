cmake_minimum_required(VERSION 3.26)
project(VulkanHelloTriangle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")


if (CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_WARN)
else ()
  add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
endif ()

include_directories(
        src/engine
        external/spdlog/include
)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(external/spdlog)

add_executable(VulkanHelloTriangle src/main.cpp
        src/engine/VulkanInstance.cpp
        src/engine/VulkanInstance.hpp
        src/engine/Window.cpp
        src/engine/Window.hpp
        src/engine/ApplicationConfig.cpp
        src/engine/ApplicationConfig.hpp
        src/engine/ValidationLayer.cpp
        src/engine/ValidationLayer.hpp
        src/Application.cpp
        src/Application.hpp
        src/engine/PhysicalDeviceSelector.cpp
        src/engine/PhysicalDeviceSelector.hpp
        src/engine/LogicalDevice.cpp
        src/engine/LogicalDevice.hpp
        src/engine/QueueFamilyUtils.cpp
        src/engine/QueueFamilyUtils.hpp
        src/engine/Surface.cpp
        src/engine/Surface.hpp
        src/engine/SwapChain.cpp
        src/engine/SwapChain.hpp
        src/engine/Abort.hpp
        src/engine/ShaderLoader.cpp
        src/engine/ShaderLoader.hpp
        src/engine/GraphicsPipeline.cpp
        src/engine/GraphicsPipeline.hpp
        src/engine/RenderPass.cpp
        src/engine/RenderPass.hpp)

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
