cmake_minimum_required(VERSION 3.8)
project(Pacman)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c game.c physics.c)
add_executable(Pacman ${SOURCE_FILES})
INCLUDE_DIRECTORIES(/usr/include/SDL2)
TARGET_LINK_LIBRARIES(Pacman m SDL2 SDL2_gfx)
ADD_DEFINITIONS(-D_REENTRANT)