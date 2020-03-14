cmake_minimum_required(VERSION 3.10)
project(GLSnippets)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O1 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG -D_RELEASE")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -s -DNDEBUG")

if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit")
endif()

# dependencies
find_package(glfw3 3.3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})


add_executable(
    window.out
    example/main.cpp
)

target_link_libraries(
    window.out
    glfw
)
