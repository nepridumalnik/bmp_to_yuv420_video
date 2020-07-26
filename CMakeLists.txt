cmake_minimum_required(VERSION 3.17)
project(bmp2yuv)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(
    SRC
    src/main.cpp
    src/image_container.cpp
)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
add_executable(${PROJECT_NAME} ${SRC})