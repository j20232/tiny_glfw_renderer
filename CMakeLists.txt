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


# box keeping aspect ratio
add_executable(
    box_keeping_aspect.out
    example/box_keeping_aspect.cpp
)

target_link_libraries(
    box_keeping_aspect.out
    glfw
    glew
)

# box keeping aspect scale
add_executable(
    box_keeping_scale.out
    example/box_keeping_scale.cpp
)

target_link_libraries(
    box_keeping_scale.out
    glfw
    glew
)
