cmake_minimum_required(VERSION 3.1.0)

project(raytracing LANGUAGES CXX)

# Set to C++17
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

find_package(SFML COMPONENTS system window graphics)


set(SRC
	src/main.cc
)

include_directories(src)

add_compile_options(-Wall -Wextra)

add_executable(rt ${SRC})
target_link_libraries(rt PRIVATE sfml-system sfml-graphics sfml-window)
