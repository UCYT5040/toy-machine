cmake_minimum_required(VERSION 3.16)
project(toy_machine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET
    sdl2 SDL2_image SDL2_ttf SDL2_mixer)

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.c" "src/emojis/*.c")
add_executable(toy_machine ${SOURCES})

target_include_directories(toy_machine PRIVATE include ${SDL2_INCLUDE_DIRS})

target_compile_options(toy_machine PRIVATE -Wall -Wextra -O2)
target_link_libraries(toy_machine PRIVATE PkgConfig::SDL2)

if(NOT WIN32)
    target_link_libraries(toy_machine PRIVATE m)
endif()

add_custom_command(TARGET toy_machine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:toy_machine>/assets"
    VERBATIM)