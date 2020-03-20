cmake_minimum_required(VERSION 3.10)
project(GLSnippets)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O1 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG -D_RELEASE")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -s -DNDEBUG")

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
endif()

if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit")
endif()
include_directories(${CMAKE_SOURCE_DIR})

# dependencies
find_package(glfw3 3.3 REQUIRED)
find_package(glew REQUIRED)


# rect keeping aspect ratio
add_executable(
    rect_keeping_aspect.out
    example/rect_keeping_aspect.cpp
)

target_link_libraries(
    rect_keeping_aspect.out
    glfw
    glew
)

# rect keeping aspect scale
add_executable(
    rect_keeping_scale.out
    example/rect_keeping_scale.cpp
)

target_link_libraries(
    rect_keeping_scale.out
    glfw
    glew
)

# rect orthognal
add_executable(
    rect_orthogonal.out
    example/rect_orthogonal.cpp
)

target_link_libraries(
    rect_orthogonal.out
    glfw
    glew
)

# rect frustum
add_executable(
    rect_frustum.out
    example/rect_frustum.cpp
)

target_link_libraries(
    rect_frustum.out
    glfw
    glew
)

# rect perspective
add_executable(
    rect_perspective.out
    example/rect_perspective.cpp
)

target_link_libraries(
    rect_perspective.out
    glfw
    glew
)

# octahedron
add_executable(
    octahedron.out
    example/octahedron.cpp
)

target_link_libraries(
    octahedron.out
    glfw
    glew
)

# box
add_executable(
    box.out
    example/box.cpp
)

target_link_libraries(
    box.out
    glfw
    glew
)
