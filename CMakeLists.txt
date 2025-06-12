cmake_minimum_required(VERSION 3.10)

project(XIF-Test)

add_subdirectory(glfw)
add_subdirectory("../CarMaker-XIF/src/XIF" "${CMAKE_BINARY_DIR}/XIF")

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
    ../CarMaker-XIF/src/XIF/include
)

target_link_libraries(XIF-Test
    imgui
    glfw
    opengl32
    ${XIF_LIBS}
)

if(DEFINED XIF_LIBS)
    add_custom_command(TARGET XIF-Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${XIF_DLL} $<TARGET_FILE_DIR:XIF-Test> 
        COMMENT "Copying ${XIF_DLL} to $<TARGET_FILE_DIR:XIF-Test> "
    )
endif()