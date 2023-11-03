cmake_minimum_required(VERSION 3.26)
project(VulkanHelloTriangle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(VulkanHelloTriangle main.cpp
        VulkanInstanceManager.cpp
        VulkanInstanceManager.hpp
        WindowManager.cpp
        WindowManager.hpp
        ApplicationConfig.cpp
        ApplicationConfig.hpp)

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
