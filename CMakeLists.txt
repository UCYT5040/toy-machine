cmake_minimum_required(VERSION 3.16)
project(toy_machine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2 SDL2_image SDL2_ttf)

file(GLOB SOURCES "src/*.c")
add_executable(toy_machine ${SOURCES})

target_include_directories(toy_machine PRIVATE include ${SDL2_INCLUDE_DIRS})

target_compile_options(toy_machine PRIVATE -Wall -Wextra -O2)
target_link_libraries(toy_machine PRIVATE ${SDL2_LIBRARIES} m)