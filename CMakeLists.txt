cmake_minimum_required(VERSION 3.27)

project(tinyrenderer)

file( GLOB SRC src/*.cpp )
file( GLOB HDR include/*.h )

add_compile_options( -std=c++17 )

add_executable( tinyrenderer main.cpp ${SRC} ${HDR} )
target_include_directories( tinyrenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/ )
