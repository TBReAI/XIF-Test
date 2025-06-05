cmake_minimum_required(VERSION 3.10)

project(XIF-Test)

add_subdirectory(glfw)

add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    imgui
    imgui/backends
    glfw/include
)

add_executable(XIF-Test
    src/main.cpp
)

target_include_directories(XIF-Test PRIVATE
    imgui
    imgui/backends
    glfw/include
)

target_link_libraries(XIF-Test
    imgui
    glfw
    opengl32
)
