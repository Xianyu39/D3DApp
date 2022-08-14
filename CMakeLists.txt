cmake_minimum_required(VERSION 3.2)
project(DX11App CXX)

set(CXX_STANDARD_REQUIRED True)
set(CXX_STANDARD 20)
set(CMAKE_WIN32_EXECUTABLE True)

add_definitions(-DUNICODE -D_UNICODE)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRCS)
add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)