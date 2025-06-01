cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0072 NEW) # Use newer OpenGL instead of legacy
set(CMAKE_CXX_STANDARD 17)


# ---PROJECT---
project(EriksWaspSim)
set(PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")


# ---SOURCES---
file(GLOB_RECURSE SOURCES
    "${PROJECT_ROOT}/src/*.cpp"
)
add_executable(EriksWaspSim ${SOURCES})


# ---INCLUDE_DIRS---
file(GLOB_RECURSE INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${PROJECT_ROOT}/src"
    "${PROJECT_ROOT}/dependencies"
)

# Filter out non-directories
foreach(dir IN LISTS INCLUDE_DIRS)
    if(NOT IS_DIRECTORY "${dir}")
        list(REMOVE_ITEM INCLUDE_DIRS "${dir}")
    endif()
endforeach()

target_include_directories(EriksWaspSim PRIVATE
    ${INCLUDE_DIRS}
)

# ---FIND PACKAGES---
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

# ---BUILD IMGUI---
set(IMGUI_DIR "${PROJECT_ROOT}/dependencies/imgui")
file(GLOB IMGUI_SOURCES
    "${IMGUI_DIR}/*.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glut.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    "${IMGUI_DIR}"
)


# ---LINK---
target_link_libraries(EriksWaspSim
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLUT_LIBRARIES}
    imgui
    readline
)

