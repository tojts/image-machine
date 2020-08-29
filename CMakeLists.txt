cmake_minimum_required (VERSION 3.8)

include_directories(lib src)

file(GLOB SRC src/*.cpp)

add_executable(imagemachine ${SRC})

