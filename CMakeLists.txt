cmake_minimum_required(VERSION 3.5)

project(game_engine)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
    src/main.cpp
    )

add_executable(GameTest ${SOURCES})

target_link_libraries(GameTest
    SDL2main 
    SDL2 
    SDL2_image)