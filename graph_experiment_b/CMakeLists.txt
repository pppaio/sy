cmake_minimum_required(VERSION 3.10)
project(ScenicGraphMatrix C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    src/graph_matrix.c
    src/console_utils.c
    src/main.c
)

add_executable(scenic_graph_matrix ${SOURCES})

set_target_properties(scenic_graph_matrix PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build
)
