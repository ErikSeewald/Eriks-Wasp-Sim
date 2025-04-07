cmake_minimum_required(VERSION 3.10)
project(Eriks_Wasp_Sim)

set(CMAKE_CXX_STANDARD 17)

# Collect source files and headers
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/*.h"
)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# Gather subdirectories under "include/" and "dependencies/"
file(GLOB_RECURSE INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/*"
)

# Filter out non-directory results
foreach(dir IN LISTS INCLUDE_DIRS)
    if(NOT IS_DIRECTORY "${dir}")
        list(REMOVE_ITEM INCLUDE_DIRS "${dir}")
    endif()
endforeach()

# Set up ImGui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/imgui-1.90.4")
file(GLOB IMGUI_SOURCES
    "${IMGUI_DIR}/*.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glut.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)
add_library(imgui STATIC ${IMGUI_SOURCES})

# Define executable
add_executable(Eriks_Wasp_Sim ${SOURCES})

# Set include directories
target_include_directories(imgui PUBLIC
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

target_include_directories(Eriks_Wasp_Sim PRIVATE
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies"
    ${INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(Eriks_Wasp_Sim
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLFW_LIBRARIES}
    imgui
    readline
)

