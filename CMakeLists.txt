cmake_minimum_required(VERSION 3.26)
project(VulkanHelloTriangle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include_directories("src/engine")

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(VulkanHelloTriangle src/main.cpp
        src/engine/VulkanInstanceManager.cpp
        src/engine/VulkanInstanceManager.hpp
        src/engine/WindowManager.cpp
        src/engine/WindowManager.hpp
        src/engine/ApplicationConfig.cpp
        src/engine/ApplicationConfig.hpp
        src/engine/ValidationLayerManager.cpp
        src/engine/ValidationLayerManager.hpp
        src/Application.cpp
        src/Application.hpp
        src/engine/PhysicalDeviceSelector.cpp
        src/engine/PhysicalDeviceSelector.hpp
        src/engine/LogicalDevice.cpp
        src/engine/LogicalDevice.hpp
        src/engine/QueueFamilyUtils.cpp
        src/engine/QueueFamilyUtils.hpp
        src/engine/Surface.cpp
        src/engine/Surface.hpp)

target_link_libraries(VulkanHelloTriangle
        PRIVATE
        glfw
        Vulkan::Vulkan
        dl
        pthread
        X11
        Xxf86vm
        Xrandr
        Xi
)
