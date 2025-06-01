cmake_minimum_required(VERSION 3.20)
project(maxrects_packer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)

add_executable(maxrects_example example.cpp)
target_link_libraries(maxrects_example maxrects_packer)

enable_testing()
add_subdirectory(tests)
